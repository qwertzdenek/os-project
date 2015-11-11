#pragma once

#include <array>
#include "sched.h"

#define BUFFER_SIZE 512

typedef enum {
	RUNNABLE, BLOCKED, RUNNING, TERMINATED
} task_state;

typedef enum {
	RUNNER, CONSUMENT, PRODUCENT
} task_type;

typedef struct {
	void* stack;
	CONTEXT context;
	int task_id;
	int quantum;
	task_state state;
	task_type type;
} task_control_block;

typedef struct
{
	semaphore_t prod_lock;
	semaphore_t cons_lock;
	semaphore_t data_lock;

	std::array<int, BUFFER_SIZE> buffer;
} task_common_pointers;

DWORD __stdcall task_main_consument(task_common_pointers *in);
DWORD __stdcall task_main_producent(task_common_pointers *in);
DWORD __stdcall task_main_runner();
DWORD __stdcall task_main_idle(void *);
