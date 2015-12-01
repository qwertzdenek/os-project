#include "stdafx.h"
#include "core.h"

#include "scheduler.h"

typedef struct sched_control_inner {
	void *stack_top;
	void *stack;

	sched_control_inner() {
		stack = _aligned_malloc(TASK_STACK_SIZE, 64);
		stack_top = (void *)((DWORD)stack + TASK_STACK_SIZE - sizeof(void *));
	}
	~sched_control_inner() { _aligned_free(stack); }
} sched_stack;

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

		; pop general registers
		popad

		; pop flags
		popfd

		ret
	}
}

__declspec(naked) void do_reschedule()
{
	__asm
	{
		call actual_core
		mov ecx, eax
		mov eax, 4; pointer is 4 bytes
		mov ebx, INTERRUPT_COUNT
		mul ebx
		mov ebx, ecx; core number
		mul ebx
		lea ebx, cpu_int_table_messages[eax + 4]
		mov esp, dword ptr[ebx]
		mov dword ptr[ebx], 0

		popad
		popfd

		ret
	}
}
