#include "stdafx.h"
#include "sched_calls.h"
#include "scheduler.h"

uint32_t exec_task(task_type type, std::shared_ptr<void> data)
{
	return sched_request_task(type, data);
}

uint32_t get_tid()
{
	return shed_get_tid();
}
