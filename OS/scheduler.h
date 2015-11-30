#pragma once

#include <queue>
#include<memory>

#include "tasks.h"
#include "cpu.h"

extern semaphore_t sched_lock;

void init_scheduler();
DWORD __stdcall scheduler_run(void *);

bool sched_active_task(int core);
void sched_store_context(int core, CONTEXT ctx);
uint32_t sched_request_task(task_type type, task_common_pointers *data);

std::string get_running_processes();
std::string get_waiting_processes();

uint32_t shed_get_tid();
