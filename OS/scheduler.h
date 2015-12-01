#pragma once

#include <queue>
#include<memory>

#include "tasks.h"
#include "cpu.h"

extern semaphore_t sched_lock;

void init_scheduler();
DWORD scheduler_run(void *);

bool sched_active_task(int core);
void sched_store_context(int core, CONTEXT ctx);
uint32_t sched_request_task(task_type type, std::shared_ptr<task_common_pointers> data);

std::string sched_get_running_tasks();
std::string sched_get_runnable_tasks();

uint32_t shed_get_tid();
