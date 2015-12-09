#include "stdafx.h"

#include <sstream>
#include <memory>
#include <deque>
#include <list>
#include <iostream>

#include "scheduler.h"
#include "tasks.h"
#include "core.h"
#include "interrupts.h"

#define TIME_QUANTUM 400
#define TIME_QUANTUM_DECREASE 50

// actual assigned tasks
static std::unique_ptr<task_control_block> running_tasks[CORE_COUNT];

// main task queue
static std::deque<std::unique_ptr<task_control_block>> task_queue;

// new task queue
static std::list<std::unique_ptr<new_task_req>> new_task_queue;

// exit task requests queue
static std::deque<std::unique_ptr<task_control_block>> exit_task_queue;

semaphore_t sched_lock;
semaphore_t sched_new_task_lock;
semaphore_t sched_stream_lock;

static uint32_t task_counter = 0;
static CONTEXT default_context;

static bool core_req_pause[CORE_COUNT];
static bool core_req_resume[CORE_COUNT];
static bool core_paused[CORE_COUNT];

void sched_end_task_callback()
{
	int core = actual_core();

	semaphore_P(sched_stream_lock, 1);
	std::cout << "task " << task_type_names[running_tasks[core]->type];
	std::cout << " id " << running_tasks[core]->task_id << " exited" << std::endl;
	semaphore_V(sched_stream_lock, 1);

	semaphore_P(sched_lock, 1);
	exit_task_queue.push_back(std::move(running_tasks[core]));

	if (task_queue.empty())
	{
		// interrupt scheduler
		semaphore_V(sched_lock, 1);
		SetEvent(cpu_int_table_handlers[0][INT_SCHEDULER]);
		SuspendThread(GetCurrentThread());
	}
	else
	{
		std::unique_ptr<task_control_block> next_task(std::move(task_queue.front()));
		task_queue.pop_front();

		DWORD target_esp = (DWORD)next_task->context.Esp;

		next_task->state = RUNNING;
		running_tasks[core] = std::move(next_task);

		semaphore_V(sched_lock, 1);

		__asm
		{
			mov esp, target_esp
			jmp do_reschedule
		}
	}
}

// will suspend thread and store his context
void sched_store_context(int core)
{
	CONTEXT ctx;
	memset(&ctx, 0, sizeof(ctx));
	ctx.ContextFlags = CONTEXT_FULL;

	if (sched_active_task(core))
	{
		SuspendThread(core_handles[core]);
		GetThreadContext(core_handles[core], &ctx);

		// push return address on task stack
		// set interrupt handler on the cpu core
		esp_push(&ctx.Esp, ctx.Eip);

		// store flags and general registers on stack here
		esp_push(&ctx.Esp, ctx.ContextFlags);
		// push dummy registers
		esp_push(&ctx.Esp, ctx.Eax);
		esp_push(&ctx.Esp, ctx.Ecx);
		esp_push(&ctx.Esp, ctx.Edx);
		esp_push(&ctx.Esp, ctx.Ebx);
		DWORD new_esp = ctx.Esp - 4 * sizeof(DWORD);
		esp_push(&ctx.Esp, new_esp);
		esp_push(&ctx.Esp, ctx.Ebp);
		esp_push(&ctx.Esp, ctx.Esi);
		esp_push(&ctx.Esp, ctx.Edi);

		running_tasks[core]->context = ctx;
	}
}

bool sched_active_task(int core)
{
	return running_tasks[core] != NULL;
}

// returns new task id
uint32_t sched_request_task(task_type type, std::shared_ptr<void> data)
{
	uint32_t task_id;
	std::unique_ptr<new_task_req> request(new new_task_req);

	request->tcp = data;
	request->type = type;
	request->task_id = task_counter++;
	task_id = request->task_id;
	
	semaphore_P(sched_new_task_lock, 1);
	new_task_queue.push_back(std::move(request));
	semaphore_V(sched_new_task_lock, 1);

	return task_id;
}

bool sched_request_pause(int core)
{
	if (!core_paused[core])
	{
		core_req_pause[core] = true;
	}
	return core_paused[core];
}

bool sched_request_resume(int core)
{
	if (core_paused[core])
	{
		core_req_resume[core] = true;
	}
	return core_paused[core];
}

uint32_t shed_get_tid()
{
	int core = actual_core();

	if (core < 0) return 0xFFFFFFFF;
	else return running_tasks[core]->task_id;
}

bool sched_task_running()
{
	bool result = false;

	for (int core = 0; core < CORE_COUNT; core++)
	{
		result |= running_tasks[core] != NULL;
	}

	return result;
}

void sched_create_task(task_control_block &tcb, new_task_req &req)
{
	tcb.context = default_context;

	tcb.context.Esp = (DWORD)tcb.stack + TASK_STACK_SIZE;
	// push argument 
	esp_push(&tcb.context.Esp, (DWORD)req.tcp.get());
	// push return address to plan next task
	esp_push(&tcb.context.Esp, (DWORD)sched_end_task_callback);
	// push argument
	esp_push(&tcb.context.Esp, (DWORD)task_entry_points[req.type]);
	// push status word
	esp_push(&tcb.context.Esp, 0x0202);
	// push dummy registers
	esp_push(&tcb.context.Esp, 0xaa); // eax
	esp_push(&tcb.context.Esp, 0xcc);
	esp_push(&tcb.context.Esp, 0xdd);
	esp_push(&tcb.context.Esp, 0xbb);
	esp_push(&tcb.context.Esp, tcb.context.Esp - 4*sizeof(DWORD));
	esp_push(&tcb.context.Esp, 0xeb);
	esp_push(&tcb.context.Esp, 0xa0);
	esp_push(&tcb.context.Esp, 0xb0); // edi

	tcb.task_id = req.task_id;
	tcb.quantum = TIME_QUANTUM;
	tcb.state = RUNNABLE;
	tcb.type = req.type;
	tcb.data = req.tcp;
}

DWORD scheduler_run(void *ptr)
{
	CONTEXT target_contexts[CORE_COUNT];
	bool context_changed[CORE_COUNT];
	memset(&context_changed, 0, CORE_COUNT * sizeof(bool));

	// pause and resume requests
	for (int core = 0; core < CORE_COUNT; core++)
	{
		if (core_req_pause[core])
		{
			core_paused[core] = true;
			task_queue.push_back(std::move(running_tasks[core]));
			core_req_pause[core] = false;
		}

		if (core_req_resume[core])
		{
			core_paused[core] = false;
			core_req_resume[core] = false;
		}
	}

	// clean up exited tasks
	while (!exit_task_queue.empty())
	{
		exit_task_queue.pop_front();
	}

	// check for new tasks
	semaphore_P(sched_new_task_lock, 1);
	while (!new_task_queue.empty())
	{
		std::unique_ptr<new_task_req> task_request(std::move(new_task_queue.front()));
		new_task_queue.pop_front();

		std::unique_ptr<task_control_block> tcb(new task_control_block);
		sched_create_task(*tcb, *task_request);
		task_queue.push_back(std::move(tcb));
	}
	semaphore_V(sched_new_task_lock, 1);

	for (int core = 0; core < CORE_COUNT; core++)
	{
		if (core_paused[core])
		{
			continue;
		}

		std::unique_ptr<task_control_block> new_task;
		std::unique_ptr<task_control_block> current_task(std::move(running_tasks[core]));

		// never trust task id
		task_control_block *current_task_ptr = current_task.get();
		
		target_contexts[core] = default_context;
		context_changed[core] = true;

		// if something is on the core
		if (current_task.get() != NULL)
		{
			current_task->quantum -= TIME_QUANTUM_DECREASE;
			if (current_task->quantum < 0)
			{
				if (current_task->type == IDLE && !task_queue.empty() && sched_task_running())
				{
					current_task.reset();
				}
				else
				{
					current_task->quantum = TIME_QUANTUM;
					current_task->state = RUNNABLE;
					task_queue.push_back(std::move(current_task));
				}
			}
			else
			{
				target_contexts[core] = current_task->context;
				running_tasks[core] = std::move(current_task);
				context_changed[core] = false;
				SetEvent(cpu_int_table_handlers[core][INT_CORE_RESUME]);
				continue;
			}
		}

		if (task_queue.empty())
		{
			if (core == 0)
			{
				// create new IDLE task
				std::unique_ptr<new_task_req> task_request(new new_task_req);
				task_request->tcp = NULL;
				task_request->type = IDLE;
				task_request->task_id = task_counter++;

				new_task.reset(new task_control_block);
				sched_create_task(*new_task, *task_request);
			}
			else
			{
				// stop 
				SetEvent(cpu_int_table_handlers[core][INT_CORE_SUSPEND]);
				context_changed[core] = false;
				continue;
			}
		}
		else
		{
			new_task = std::move(task_queue.front());
			task_queue.pop_front();
		}

		if (current_task_ptr != NULL && new_task.get() == current_task_ptr)
		{
			context_changed[core] = false;
		}
		else
		{
			context_changed[core] = true;
		}

		new_task->state = RUNNING;
		target_contexts[core] = new_task->context;
		running_tasks[core] = std::move(new_task);
	}

	// send final reschedule events to the other cores
	for (int core = 1; core < CORE_COUNT; core++)
	{
		if (context_changed[core])
		{
			cpu_int_table_messages[core][INT_RESCHEDULE] = (void *)target_contexts[core].Esp;
			SetEvent(cpu_int_table_handlers[core][INT_RESCHEDULE]);
		}
	}

	return target_contexts[0].Esp;
}

void init_scheduler()
{
	memset(&default_context, 0, sizeof(default_context));
	HANDLE CPUCore = (HANDLE)CreateThread(NULL, 0, NULL, 0, CREATE_SUSPENDED, NULL);

	default_context.ContextFlags = CONTEXT_FULL;
	GetThreadContext(CPUCore, &default_context);

	memset(core_req_pause, 0, CORE_COUNT*sizeof(bool));
	memset(core_req_resume, 0, CORE_COUNT*sizeof(bool));
	memset(core_paused, 0, CORE_COUNT*sizeof(bool));

	SetEvent(cpu_int_table_handlers[0][INT_SCHEDULER]);
}

std::string sched_get_running_tasks()
{
	std::stringstream ss;

	semaphore_P(sched_lock, 1);
	for (int i = 0; i < CORE_COUNT; i++)
	{
		if (sched_active_task(i))
		{
			ss << i << "| ";
			ss << running_tasks[i]->task_id << ' ';
			ss << task_state_names[running_tasks[i]->state] << ' ';
			ss << task_type_names[running_tasks[i]->type] << '\n';
		}
	}
	semaphore_V(sched_lock, 1);

	return ss.str();
}

std::string sched_get_runnable_tasks()
{
	std::stringstream ss;

	semaphore_P(sched_lock, 1);
	for (size_t i = 0; i < task_queue.size(); i++)
	{
		ss << task_queue[i]->task_id << ' ';
		ss << task_state_names[task_queue[i]->state] << ' ';
		ss << task_type_names[task_queue[i]->type] << '\n';
	}
	semaphore_V(sched_lock, 1);

	return ss.str();
}

std::string sched_get_cores_info()
{
	std::stringstream ss;

	semaphore_P(sched_lock, 1);
	for (int i = 0; i < CORE_COUNT; i++)
	{
		if (running_tasks[i] != NULL)
		{
			ss << "core " << i << " occupied" << '\n';
		}
		else {
			ss << "core " << i << " free" << '\n';
		}
	}
	semaphore_V(sched_lock, 1);

	return ss.str();
}

std::string sched_get_tasks_progress()
{
	std::stringstream ss;

	semaphore_P(sched_lock, 1);

	for (size_t i = 0; i < task_queue.size(); i++)
	{
		if (task_queue[i]->type == PRODUCENT || task_queue[i]->type == CONSUMENT)
		{
			ss << "planned task " << task_queue[i]->task_id << " difference |";
			ss << " mean: " << reinterpret_cast<task_common_pointers*>(task_queue[i]->data.get())->mean_diff;
			ss << " deviation: " << reinterpret_cast<task_common_pointers*>(task_queue[i]->data.get())->deviation_diff << '\n';
		}
	}

	for (int i = 0; i < CORE_COUNT; i++)
	{
		if (running_tasks[i] != NULL &&
			(running_tasks[i]->type == PRODUCENT || running_tasks[i]->type == CONSUMENT))
		{
			ss << "actual task " << running_tasks[i]->task_id << " difference |";
			ss << " mean: " << reinterpret_cast<task_common_pointers*>(running_tasks[i]->data.get())->mean_diff;
			ss << " deviation: " << reinterpret_cast<task_common_pointers*>(running_tasks[i]->data.get())->deviation_diff << '\n';
		}
	}

	semaphore_V(sched_lock, 1);

	return ss.str();
}
