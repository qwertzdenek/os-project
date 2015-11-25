#include "stdafx.h"
#include "sched_calls.h"

#include "tasks.h"

void *task_entry_points[4] = { task_main_runner, task_main_consument,
							   task_main_producent, task_main_idle};

DWORD __stdcall task_main_idle(void *)
{
	while (1)
		;
}

DWORD __stdcall task_main_runner(void *in)
{
	// init common memory and call exec_task
	task_common_pointers *struct_ptr = new task_common_pointers;

	int prod_id = exec_task(PRODUCENT, struct_ptr);
	int cons_id = exec_task(CONSUMENT, struct_ptr);

	// wait for them to exit
	wait_task(prod_id);
	wait_task(cons_id);

	exit_task();
	return 0;
}

DWORD __stdcall task_main_producent(void *in)
{
	// do something

	exit_task();
	return 0;
}

DWORD __stdcall task_main_consument(void *in)
{
	// do something

	exit_task();
	return 0;
}
