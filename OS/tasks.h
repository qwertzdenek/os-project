#pragma once

#include <array>
#include <memory>
#include <cmath>
#include <random>

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "buffer.h"
#include "synchro.h"

#define PRECISION 0.01

#define TASK_STACK_SIZE 1024*1024

typedef enum {
	RUNNABLE, BLOCKED, RUNNING, TERMINATED
} task_state;

typedef enum {
	RUNNER, CONSUMENT, PRODUCENT, IDLE
} task_type;

struct task_common_pointers {
	semaphore_t full;
	semaphore_t empty;
	semaphore_t mutex;

	circular_buffer buffer;

    volatile bool can_run;
    double mean;
    double deviation;
};

struct task_run_parameters {
    double mean;
    double deviation;
};

struct task_control_block {
	void* stack;
	CONTEXT context;
	uint32_t task_id;
	int quantum;
	task_state state;
	task_type type;
	std::shared_ptr<task_common_pointers> data;

	task_control_block() { stack = _aligned_malloc(TASK_STACK_SIZE, 64); }
	~task_control_block() { _aligned_free(stack); }
};

struct new_task_req {
	uint32_t task_id;
	task_type type;
	std::shared_ptr<task_common_pointers> tcp;
};

DWORD task_main_consument(void *);
DWORD task_main_producent(void *);
DWORD task_main_runner(void *);
DWORD task_main_idle(void *);

// mapping from the task_type struct to pointers
extern void *task_entry_points[4];

extern const std::string task_state_names[];

extern const std::string task_type_names[];
