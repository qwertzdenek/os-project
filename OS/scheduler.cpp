#include "stdafx.h"

#include<queue>

#include "scheduler.h"
#include "tasks.h"
#include "core.h"

#define NUMBER_OF_TICKS_TO_RESCHEDULE 100
#define TIME_QUANTUM 25
#define TIME_QUANTUM_DECREASE 25

// actual assigned tasks
task_control_block *running_tasks[CORE_COUNT];

// main task queue
std::queue<task_control_block> task_queue;

// main task queue
// TODO: add task_common_pointers
std::queue<task_type> new_task_queue;

unsigned long tick_count = 0;
int task_counter = 0;
CONTEXT default_context;

void create_task(task_control_block *tcb, task_type type)
{
	tcb->context = default_context;
	tcb->stack = _aligned_malloc(THREAD_STACK_SIZE, 64);
	tcb->context.Esp = (DWORD32)tcb->stack + THREAD_STACK_SIZE - sizeof(DWORD32);
	tcb->task_id = task_counter++;
	tcb->quantum = TIME_QUANTUM;
	tcb->state = RUNNABLE;
	tcb->type = type;
}

void scheduler_run()
{
	// check for new tasks
	while (!new_task_queue.empty())
	{
		task_type task = new_task_queue.front();
		new_task_queue.pop();

		task_control_block *tcb = new task_control_block;
		create_task(tcb, task);
		task_queue.push(*tcb);
	}

	for (int core = 0; core < CORE_COUNT; core++)
	{
		// lower all scheduled task their quantum

		// if you want to reschedule to another core, store address of
		// tcb on messages table and interrupt it
		// -> SetEvent(cpu_int_table_handlers[core][1])
	}
}

void init_scheduler()
{
	memset(&default_context, 0, sizeof(default_context));
	HANDLE CPUCore = (HANDLE)CreateThread(NULL, 0, task_main_idle, 0, CREATE_SUSPENDED, NULL);

	default_context.ContextFlags = CONTEXT_ALL;
	GetThreadContext(CPUCore, &default_context);
}
