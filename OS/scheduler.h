#pragma once

#include <queue>
#include<memory>

#include "tasks.h"

void init_scheduler();
DWORD __stdcall scheduler_run();

void sched_store_context(int core, CONTEXT ctx);
int sched_request_task(task_type type, task_common_pointers *data);
void sched_request_exit(int core_number);
int shed_get_tid();
