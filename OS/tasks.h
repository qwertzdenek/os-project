#pragma once

#include <array>
#include <memory>
#include "sched.h"
#include "buffer.h"

#define TASK_STACK_SIZE 1024

typedef enum {
	RUNNABLE, BLOCKED, RUNNING, TERMINATED
} task_state;

typedef enum {
	RUNNER, CONSUMENT, PRODUCENT, IDLE
} task_type;

typedef struct task_control_block_inner {
	void* stack;
	CONTEXT context;
	int task_id;
	int quantum;
	task_state state;
	task_type type;

	task_control_block_inner() { stack = _aligned_malloc(TASK_STACK_SIZE, 64); }
	~task_control_block_inner() { _aligned_free(stack); }
} task_control_block;

typedef struct
{
	semaphore_t full;
	semaphore_t empty;
	semaphore_t mutex;
	
	Buffer buffer;
} task_common_pointers;

typedef struct {
	int task_id;
	task_type type;
	std::unique_ptr<task_common_pointers> tcp;
} new_task_req;

DWORD __stdcall task_main_consument(task_common_pointers *in);
DWORD __stdcall task_main_producent(task_common_pointers *in);
DWORD __stdcall task_main_runner();
DWORD __stdcall task_main_idle(void *);

// mapping from the task_type struct to pointers
extern void *task_entry_points[4];
