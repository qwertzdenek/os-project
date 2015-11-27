#pragma once

#include <queue>
#include<memory>

#include "tasks.h"
#include "cpu.h"

extern semaphore_t sched_lock;

void init_scheduler();
DWORD __stdcall scheduler_run();

bool sched_active_task(int core);
void sched_store_context(int core, CONTEXT ctx);
int sched_request_task(task_type type, task_common_pointers *data);

uint32_t shed_get_tid();
extern std::deque<std::unique_ptr<task_control_block>> task_queue;
extern std::unique_ptr<task_control_block> running_tasks[CORE_COUNT];
