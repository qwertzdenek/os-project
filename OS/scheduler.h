#pragma once

#include <queue>
#include<memory>

#include "tasks.h"

extern semaphore_t sched_lock;

void init_scheduler();
DWORD __stdcall scheduler_run();

bool sched_active_task(int core);
void sched_store_context(int core, CONTEXT ctx);
int sched_request_task(task_type type, task_common_pointers *data);
uint32_t shed_get_tid();
