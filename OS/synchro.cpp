#include "stdafx.h"

#include "synchro.h"
#include "core.h"

// release semaphore and return his value
void semaphore_V(semaphore_t &s, int value)
{
	// it's already atomic
	s._value += value;
}

int semaphore_P(semaphore_t &s, int value)
{
	int expected;
	int old;

	do {
		do {
			old = s._value;
			expected = old - value;
		} while (expected < 0);
	} while (!s._value.compare_exchange_weak(old, expected,
		std::memory_order_release,
		std::memory_order_relaxed));

	return expected;
}
