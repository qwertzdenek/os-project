#include "stdafx.h"

#include <memory>
#include <mutex>
#include <deque>
#include <list>

#include "scheduler.h"
#include "tasks.h"
#include "core.h"
#include "interrupts.h"


#define TIME_QUANTUM 100
#define TIME_QUANTUM_DECREASE 25

// actual assigned tasks
static std::unique_ptr<task_control_block> running_tasks[CORE_COUNT];

// main task queue
static std::deque<std::unique_ptr<task_control_block>> task_queue;

// new task queue
static std::list<new_task_req *> new_task_queue;

// exit task requests queue
static std::deque<std::unique_ptr<task_control_block>> exit_task_queue;

semaphore_t sched_lock;

static uint32_t task_counter = 0;
static CONTEXT default_context;

void sched_end_task_callback()
{
	int core = actual_core();

	semaphore_P(sched_lock, 1);
	exit_task_queue.push_back(std::move(running_tasks[core]));

	if (task_queue.empty())
	{
		// interrupt scheduler
		SetEvent(cpu_int_table_handlers[0][INT_SCHEDULER]);
		semaphore_V(sched_lock, 1);
		SuspendThread(GetCurrentThread());
	}
	else
	{
		std::unique_ptr<task_control_block> next_task(std::move(task_queue.front()));
		task_queue.pop_front();

		cpu_int_table_messages[core][1] = (void *)next_task->context.Esp;

		next_task->state = RUNNING;
		running_tasks[core] = std::move(next_task);
		semaphore_V(sched_lock, 1);
		do_reschedule();
	}
}

void sched_store_context(int core, CONTEXT ctx)
{
	running_tasks[core]->context = ctx;
}

bool sched_active_task(int core)
{
	return running_tasks[core] != NULL;
}

// returns new task id
uint32_t sched_request_task(task_type type, task_common_pointers *data)
{
	uint32_t task_id;
	new_task_req *request = new new_task_req;

	request->tcp.reset(data);
	request->type = type;
	request->task_id = task_counter++;
	task_id = request->task_id;
	
	semaphore_P(sched_lock, 1);
	new_task_queue.push_back(request);
	semaphore_V(sched_lock, 1);

	return task_id;
}

uint32_t shed_get_tid()
{
	return running_tasks[actual_core()]->task_id;
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
}

DWORD __stdcall scheduler_run()
{
	CONTEXT target_contexts[CORE_COUNT];
	bool context_changed[CORE_COUNT];
	memset(&context_changed, 0, CORE_COUNT * sizeof(bool));

	// clean up exited tasks
	while (!exit_task_queue.empty())
	{
		exit_task_queue.pop_front();
	}

	// check for new tasks
	while (!new_task_queue.empty())
	{
		std::unique_ptr<new_task_req> task_request(new_task_queue.front());
		new_task_queue.pop_front();

		std::unique_ptr<task_control_block> tcb(new task_control_block);
		sched_create_task(*tcb, *task_request);
		task_queue.push_back(std::move(tcb));
	}

	for (int core = 0; core < CORE_COUNT; core++)
	{
		std::unique_ptr<task_control_block> new_task;
		std::unique_ptr<task_control_block> current_task(std::move(running_tasks[core]));

		target_contexts[core] = default_context;
		context_changed[core] = true;

		// if something is on the core
		if (current_task.get() != NULL)
		{
			current_task->quantum -= TIME_QUANTUM_DECREASE;
			if (current_task->quantum < 0)
			{
				current_task->quantum = TIME_QUANTUM;
				current_task->state = RUNNABLE;
				task_queue.push_back(std::move(current_task));
			}
			else
			{
				target_contexts[core] = current_task->context;
				running_tasks[core] = std::move(current_task);
				context_changed[core] = false;
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

		new_task->state = RUNNING;

		context_changed[core] = true;
		target_contexts[core] = new_task->context;

		running_tasks[core] = std::move(new_task);
	}

	// send final reschedule events to the other cores
	for (int core = 1; core < CORE_COUNT; core++)
	{
		if (context_changed[core])
		{
			cpu_int_table_messages[core][1] = (void *)target_contexts[core].Esp;
			SetEvent(cpu_int_table_handlers[core][INT_RESCHEDULE]);
		}
	}

	semaphore_V(sched_lock, 1);
	return target_contexts[0].Esp;
}

void init_scheduler()
{
	memset(&default_context, 0, sizeof(default_context));
	HANDLE CPUCore = (HANDLE)CreateThread(NULL, 0, task_main_idle, 0, CREATE_SUSPENDED, NULL);

	default_context.ContextFlags = CONTEXT_FULL;
	GetThreadContext(CPUCore, &default_context);

	// initialize first idle task on first core
	new_task_req task_request;
	std::unique_ptr<task_control_block> tcb(new task_control_block);

	task_request.tcp = NULL;
	task_request.type = IDLE;
	task_request.task_id = task_counter++;

	sched_create_task(*tcb, task_request);
	tcb->quantum = 0;
	running_tasks[0] = std::move(tcb);

	sched_lock._value = 1;

	SetEvent(cpu_int_table_handlers[0][INT_CORE_RESUME]);
}
