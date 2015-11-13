#include "stdafx.h"
#include "sched_calls.h"

// argv will be NULL terminated list
int exec_task(task_type type, task_common_pointers *data)
{
	// TODO
	return 0;
}

int wait_task(int task_id)
{
	// TODO
	return 0;
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
}

// release semaphore and return his value

void semaphore_V(semaphore_t &s) {
    s._value++;
}

// exits the calling process
void exit_task(int result_code)
{
	// TODO
}
