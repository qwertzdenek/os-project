#include "stdafx.h"
#include "sched_calls.h"
#include "scheduler.h"

uint32_t exec_task(task_type type, std::shared_ptr<task_common_pointers> data)
{
	return sched_request_task(type, data);
}

uint32_t wait_task(uint32_t task_id)
{
	// TODO
	return 0;
}

uint32_t get_tid()
{
	return shed_get_tid();
}
