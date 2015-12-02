#pragma once

#include <atomic>

typedef struct {
	int _value = 1;
	int _core;
} semaphore_t;

// try accuire semaphore and return his value
bool try_semaphore_P(semaphore_t &s, int value);

// accuire semaphore and return his value
int semaphore_P(semaphore_t &s, int value);

// release semaphore and return his value
void semaphore_V(semaphore_t &s, int value);
