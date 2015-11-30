#pragma once

#include <array>
#include <memory>
#include "buffer.h"
#include "synchro.h"

#define TASK_STACK_SIZE 1024*1024

typedef enum {
	RUNNABLE, BLOCKED, RUNNING, TERMINATED
} task_state;

typedef enum {
	RUNNER, CONSUMENT, PRODUCENT, IDLE
} task_type;

struct task_control_block {
	void* stack;
	CONTEXT context;
	uint32_t task_id;
	int quantum;
	task_state state;
	task_type type;

	task_control_block() { stack = _aligned_malloc(TASK_STACK_SIZE, 64); }
	~task_control_block() { _aligned_free(stack); }
};

struct task_common_pointers {
	semaphore_t full;
	semaphore_t empty;
	semaphore_t mutex;

	circular_buffer buffer;
};

struct new_task_req {
	uint32_t task_id;
	task_type type;
	std::unique_ptr<task_common_pointers> tcp;
};

DWORD __stdcall task_main_consument(void *);
DWORD __stdcall task_main_producent(void *);
DWORD __stdcall task_main_runner(void *);
DWORD __stdcall task_main_idle(void *);

// mapping from the task_type struct to pointers
extern void *task_entry_points[4];

extern const std::string task_state_names[];

extern const std::string task_type_names[];
