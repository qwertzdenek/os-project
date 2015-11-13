#pragma once

#include <atomic>
#include <mutex>

typedef struct {
    std::atomic<int> _value;
    std::atomic<bool> _PMutexAquired;
} semaphore_t;
