#pragma once

#include "stdafx.h"

typedef enum {
	RUNNABLE, BLOCKED, RUNNING, TERMINATED
} task_state;

typedef struct {
	CONTEXT context;
	HANDLE stack_handle;
	void* stack;
	int task_id;
	time_t started;
	task_state state;
} task_control_block;
