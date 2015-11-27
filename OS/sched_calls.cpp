#include "stdafx.h"
#include "sched_calls.h"
#include "scheduler.h"

int exec_task(task_type type, task_common_pointers *data)
{
	return sched_request_task(type, data);
}

int wait_task(int task_id)
{
	// TODO
	return 0;
}

uint32_t get_tid()
{
	return shed_get_tid();
}
