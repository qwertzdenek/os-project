#include "stdafx.h"
#include "sched_calls.h"

#include "task_common.h"

void task_main_runner()
{
	// init common memory and call exec_task
	task_common_pointers *struct_ptr = new task_common_pointers;

	int prod_id = exec_task("producent", struct_ptr);
	int cons_id = exec_task("consument", struct_ptr);

	// wait for them to exit
	wait_task(prod_id);
	wait_task(cons_id);

	delete struct_ptr;

	exit_task(0);
}
