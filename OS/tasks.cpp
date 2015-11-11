#include "stdafx.h"
#include "sched_calls.h"

#include "tasks.h"

DWORD __stdcall task_main_idle(void *)
{
	while (1)
		;
}

DWORD __stdcall task_main_runner()
{
	// init common memory and call exec_task
	task_common_pointers *struct_ptr = new task_common_pointers;

	int prod_id = exec_task(PRODUCENT, struct_ptr);
	int cons_id = exec_task(CONSUMENT, struct_ptr);

	// wait for them to exit
	wait_task(prod_id);
	wait_task(cons_id);

	delete struct_ptr;

	exit_task(0);
	return 0;
}

DWORD __stdcall task_main_producent(task_common_pointers *in)
{
	// do something

	exit_task(0);
	return 0;
}

DWORD __stdcall task_main_consument(task_common_pointers *in)
{
	// do something

	exit_task(0);
	return 0;
}
