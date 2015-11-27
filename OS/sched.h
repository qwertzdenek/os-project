#pragma once

#include <atomic>

typedef struct {
    std::atomic<int> _value = 1;
	std::atomic<uint8_t> _core;
} semaphore_t;
