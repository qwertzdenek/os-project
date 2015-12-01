#include "stdafx.h"

#include "synchro.h"
#include "core.h"

// release semaphore and return his value
void __stdcall semaphore_V(semaphore_t &s, int value)
{
	s._value += value;
}

// accuire semaphore and return his value
int __stdcall semaphore_P(semaphore_t &s, int value)
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

	// store locked core
	s._core = actual_core();

	return expected;
}
