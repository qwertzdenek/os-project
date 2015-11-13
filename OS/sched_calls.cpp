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

// release semaphore and return his value
void semaphore_V(semaphore_t &s) {
    s._value++;
}

// accuire semaphore and return his value
int semaphore_P(semaphore_t &s) {
    bool exchanged = false;
    int value;
	bool new_bool = true;
	bool &ref_new_bool = new_bool;

    printf("Current semaphore value: %d", s._value.load());
    // wait for semafore to have free space
    while (s._value <= 0) {
    }

    while (!exchanged) {

        printf("Is semaphore in use: %s", s._PMutexAquired ? "true" : "false");
        // wait for last writer to release mutex
        while (s._PMutexAquired) {
        }

        exchanged = s._PMutexAquired.compare_exchange_weak(ref_new_bool, false);
		
        if (exchanged) {
            printf("mutex was aquired");
            // wait till some space is released
            while (s._value <= 0) {
            }

            value = --(s._value);
        }
    }

    return value;
void exit_task()
{
	sched_request_exit(get_tid());
}

int get_tid()
{
	return shed_get_tid();
}
