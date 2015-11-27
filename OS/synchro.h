#pragma once

#include <atomic>

typedef struct {
	std::atomic<int> _value = 1;
	std::atomic<uint8_t> _core;
} semaphore_t;

// accuire semaphore and return his value
int semaphore_P(semaphore_t &s, int value);

// release semaphore and return his value
void semaphore_V(semaphore_t &s, int value);
