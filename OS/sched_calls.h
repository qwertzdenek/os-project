#pragma once

#include "tasks.h"

// it will add this task to the exec list and return descriptor
int exec_task(task_type type, task_common_pointers *data);

// waits for the task with id and returns return code
int wait_task(int task_id);

// get actual task id
uint32_t get_tid();
