#pragma once

#include <mutex>
#include <deque>

typedef struct {
	int _semaphore_value;
	std::deque<int> _waiting;
	std::mutex _waiting_lock;
} semaphore_t;
