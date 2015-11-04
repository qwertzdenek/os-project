#pragma once

#include <vector>
#include <mutex>
#include <queue>

typedef struct {
	int _semaphore_value;
	std::vector<void *> waiting;
	std::mutex waiting_lock;
} semaphore_t;
