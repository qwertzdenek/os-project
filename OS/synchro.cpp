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
	int result;

	__asm
	{
	spin:
		mov edx, s
		mov edx, [edx]s._value
		mov ebx, edx
		sub ebx, value
		js spin

		mov eax, edx
		// eax - old, [s], ebx - expected
		mov ecx, s
		lock cmpxchg[ecx]s._value, ebx
		jnz spin
		mov result, ebx

		call actual_core
		mov ebx, s
		mov[ebx]s._core, eax
	}

	return result;
}

bool try_semaphore_P(semaphore_t &s, int value)
{
	bool result;
	__asm
	{
	try_spin:
		mov edx, s
		mov edx, [edx]s._value
		mov ebx, edx
		sub ebx, value
		js try_fail

		mov eax, edx
		// eax - old, [s], ebx - expected
		mov ecx, s
		lock cmpxchg[ecx]s._value, ebx
		jnz try_spin
		jmp finish
	try_fail :
		mov result, 0
		jmp final_end
	finish :
		call actual_core
		mov ebx, s
		mov[ebx]s._core, eax
		mov result, 1
	final_end:
	}

	return result;
}
