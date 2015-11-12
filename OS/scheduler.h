#pragma once

#include "tasks.h"

void init_scheduler();
void scheduler_run();
void run_task(task_control_block newTask, int core);
