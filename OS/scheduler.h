#pragma once

#include <queue>
#include<memory>

#include "tasks.h"

void init_scheduler();
DWORD scheduler_run();

int sched_request_task(task_type type, task_common_pointers *data);
void sched_request_exit(int task_id);
int shed_get_tid();
