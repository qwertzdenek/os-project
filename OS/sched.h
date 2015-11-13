#pragma once

#include <atomic>

typedef struct {
    std::atomic<int> _value;
} semaphore_t;
