#pragma once

#include <queue>
#include<memory>

#include "tasks.h"
#include "cpu.h"

extern semaphore_t sched_lock;

void init_scheduler();
DWORD scheduler_run(void *);

bool sched_active_task(int core);
void sched_store_context(int core);
uint32_t sched_request_task(task_type type, std::shared_ptr<void> data);
bool sched_request_pause(int core);
bool sched_request_resume(int core);

std::string sched_get_running_tasks();
std::string sched_get_runnable_tasks();
std::string sched_get_cores_info();

uint32_t shed_get_tid();
