#include "stdafx.h"

#include "scheduler.h"
#include "tasks.h"
#include "core.h"

#define TIME_QUANTUM 100
#define TIME_QUANTUM_DECREASE 25

// actual assigned tasks
std::unique_ptr<task_control_block> running_tasks[CORE_COUNT];

// main task queue
std::queue<std::unique_ptr<task_control_block>> task_queue;

// new task queue
std::queue<std::unique_ptr<new_task_req>> new_task_queue;

// exit task requests queue
std::queue<int> exit_task_queue;

unsigned long tick_count = 0;
int task_counter = 0;
CONTEXT default_context;

void esp_push(DWORD *esp, DWORD value)
{
	*esp -= sizeof(DWORD);
	*(DWORD *)*esp = value;
}

DWORD esp_pop(DWORD *esp)
{
	DWORD value = *(DWORD *)*esp;
	*esp += sizeof(DWORD);
	return value;
}

void __stdcall update_esp(DWORD esp)
{
	int core = actual_core();
	if (running_tasks[core] != NULL)
	{
		running_tasks[core]->stack = (void *)esp;
	}
}

void __stdcall load_esp()
{
	int core = actual_core();
	if (running_tasks[core] != NULL)
	{
		running_tasks[core]->stack = (void *)esp;
	}
}

// returns new task id
int sched_request_task(task_type type, task_common_pointers *data)
{
	std::unique_ptr<new_task_req> request(new new_task_req);
	request->tcp.reset(data);
	request->type = type;
	request->task_id = task_counter++;
	new_task_queue.push(std::move(request));

	return request->task_id;
}

void sched_request_exit(int task_id)
{
	exit_task_queue.push(task_id);
}

int shed_get_tid()
{
	return running_tasks[actual_core()]->task_id;
}

void sched_create_task(task_control_block &tcb, new_task_req &req)
{
	tcb.context = default_context;
	tcb.stack = _aligned_malloc(THREAD_STACK_SIZE, 64);

	tcb.context.Esp = (DWORD)tcb.stack + THREAD_STACK_SIZE;
	// push argument 
	esp_push(&tcb.context.Esp, (DWORD)req.tcp.get());
	// push return address to idle task
	esp_push(&tcb.context.Esp, (DWORD)task_entry_points[IDLE]);
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

	tcb.stack = (void *) tcb.context.Esp;

	tcb.task_id = req.task_id;
	tcb.quantum = TIME_QUANTUM;
	tcb.state = RUNNABLE;
	tcb.type = req.type;
}

DWORD scheduler_run()
{
	CONTEXT zero_core_context = default_context;

	// check for new tasks
	while (!new_task_queue.empty())
	{
		std::unique_ptr<new_task_req> task_request(std::move(new_task_queue.front()));
		new_task_queue.pop();

		std::unique_ptr<task_control_block> tcb(new task_control_block);
		sched_create_task(*tcb, *task_request);
		task_queue.push(std::move(tcb));
	}

	for (int core = 0; core < CORE_COUNT; core++)
	{
		std::unique_ptr<task_control_block> current_task(std::move(running_tasks[core]));
		if (current_task == NULL)
			continue;

		current_task->quantum -= TIME_QUANTUM_DECREASE;
		if (current_task->quantum <= 0)
		{
			std::unique_ptr<task_control_block> new_task;

			if (task_queue.empty())
			{
				current_task->quantum = TIME_QUANTUM;
				new_task = std::move(current_task);
			}
			else
			{
				current_task->state = RUNNABLE;
				new_task = std::move(task_queue.front());
				task_queue.pop();

				task_queue.push(std::move(current_task));
			}

			new_task->state = RUNNING;

			if (core == 0)
			{
				zero_core_context = new_task->context;
			}
			else
			{
				cpu_int_table_messages[core][1] = new_task->stack;
				SetEvent(cpu_int_table_handlers[core][1]);
			}

			running_tasks[core] = std::move(new_task);
		}
	}

	return zero_core_context.Esp;
}

void init_scheduler()
{
	memset(&default_context, 0, sizeof(default_context));
	HANDLE CPUCore = (HANDLE)CreateThread(NULL, 0, task_main_idle, 0, CREATE_SUSPENDED, NULL);

	default_context.ContextFlags = CONTEXT_ALL;
	GetThreadContext(CPUCore, &default_context);

	// initialize first idle task on first core
	std::unique_ptr<new_task_req> task_request(new new_task_req);
	std::unique_ptr<task_control_block> tcb(new task_control_block);

	task_request->tcp = NULL;
	task_request->type = IDLE;
	task_request->task_id = task_counter++;

	sched_create_task(*tcb, *task_request);
	tcb->quantum = 0;
	running_tasks[0] = std::move(tcb);
}
