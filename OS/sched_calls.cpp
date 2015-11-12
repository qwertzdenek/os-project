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

void exit_task(int result_code)
{
	sched_request_exit(get_tid());
}

int get_tid()
{
	// TODO
}

int semaphore_P(semaphore_t &s)
{
	// TODO
	return 0;
}

void semaphore_V(semaphore_t &s)
{
	// TODO
}
