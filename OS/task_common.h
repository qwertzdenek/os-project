#pragma once

#include <array>
#include "sched.h"

#define BUFFER_SIZE 512

typedef struct
{
	semaphore_t prod_lock;
	semaphore_t cons_lock;
	semaphore_t data_lock;

	std::array<int, BUFFER_SIZE> buffer;
} task_common_pointers;
