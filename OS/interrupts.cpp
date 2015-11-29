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

void do_schedule()
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

void do_reschedule()
{
	void *stack = cpu_int_table_messages[actual_core()][1];

	__asm
	{
		mov esp, dword ptr[stack]
		mov dword ptr[stack], 0

		popad
		popfd

		ret
	}
}
