#pragma once

#include <string>

#include "sched.h"
#include "tasks.h"

// it will add this task to the exec list and return descriptor
int exec_task(task_type type, task_common_pointers *data);

// waits for the task with id and returns return code
int wait_task(int task_id);

// exits the calling process
void exit_task();

// get actual task id
int get_tid();

// accuire semaphore and return his value
int semaphore_P(semaphore_t &s, int value);

// release semaphore and return his value
void semaphore_V(semaphore_t &s, int value);
