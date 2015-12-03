#include "stdafx.h"
#include "core.h"

#include "scheduler.h"

struct sched_stack {
	void *stack_top;
	void *stack;

	sched_stack() {
		stack = _aligned_malloc(TASK_STACK_SIZE, 64);
		stack_top = (void *)((DWORD)stack + TASK_STACK_SIZE - sizeof(void *));
	}
	~sched_stack() { _aligned_free(stack); }
};

static sched_stack sc;

__declspec(naked) void do_schedule()
{
	// this will run only on the first core
	__asm
	{
		lea ebx, sc
		mov esp, [ebx].stack_top

		call scheduler_run
		mov esp, eax
	}

	semaphore_V(sched_lock, 1);

	__asm
	{
		; pop general registers
		popad

		; pop flags
		popfd

		ret
	}
}

// set ecx to target core before
__declspec(naked) void do_reschedule()
{
	__asm
	{
		mov eax, 4; pointer is 4 bytes
		mov ebx, INTERRUPT_COUNT
		mul ebx
		mov ebx, ecx
		mul ebx
		lea ebx, cpu_int_table_messages[eax + 4]
		mov esp, dword ptr[ebx]
	}

	semaphore_V(sched_lock, 1);

	__asm
	{
		popad
		popfd

		ret
	}
}
