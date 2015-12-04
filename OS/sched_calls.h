#pragma once

#include "tasks.h"

// it will add this task to the exec list and return descriptor
uint32_t exec_task(task_type type, std::shared_ptr<void> data);

// waits for the task with id and returns return code
uint32_t wait_task(uint32_t task_id);

// get actual task id
uint32_t get_tid();
