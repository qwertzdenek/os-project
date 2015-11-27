#include "stdafx.h"
#include "sched_calls.h"
#include "scheduler.h"
#include "core.h"

int exec_task(task_type type, task_common_pointers *data)
{
	return sched_request_task(type, data);
}

int wait_task(int task_id)
{
	// TODO
	return 0;
}

uint8_t get_tid()
{
	return shed_get_tid();
}

// release semaphore and return his value
void semaphore_V(semaphore_t &s, int value) 
{
    s._value += value;
}

// accuire semaphore and return his value
int semaphore_P(semaphore_t &s, int value) 
{
    int expected;
    int old;

    do {
        do {
            old = s._value;
            expected = old - value;
        } while (expected < 0);
    } while (!s._value.compare_exchange_weak(old, expected,
            std::memory_order_release,
            std::memory_order_relaxed));

	// store locked core
	s._core = actual_core();

    return expected;
}
