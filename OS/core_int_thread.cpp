#include "stdafx.h"

#include "core_int_thread.h"
#include "scheduler.h"
#include "core.h"
#include "sched_calls.h"

// interrupt table
bool cpu_int_table_masked[CORE_COUNT][INTERRUPT_COUNT];
void *cpu_int_table_routines[CORE_COUNT][INTERRUPT_COUNT];
HANDLE cpu_int_table_handlers[CORE_COUNT][INTERRUPT_COUNT];
void *cpu_int_table_messages[CORE_COUNT][INTERRUPT_COUNT];

// controls int threads
bool int_thread_running[CORE_COUNT];

void core_do_interrupt(void *entry_point, int core_number)
{
	CONTEXT ctx;
	memset(&ctx, 0, sizeof(ctx));
	ctx.ContextFlags = CONTEXT_FULL;

	HANDLE target_core = core_handles[core_number];

	SuspendThread(target_core);
	GetThreadContext(target_core, &ctx);
	
	if (sched_active_task(core_number))
	{
		// push return address on task stack
		// set interrupt handler on the cpu core
		ctx.Esp -= sizeof(DWORD32);
		*(DWORD32 *)ctx.Esp = ctx.Eip;

		// store flags and general registers on stack here
		esp_push(&ctx.Esp, ctx.ContextFlags);
		// push dummy registers
		esp_push(&ctx.Esp, ctx.Eax);
		esp_push(&ctx.Esp, ctx.Ecx);
		esp_push(&ctx.Esp, ctx.Edx);
		esp_push(&ctx.Esp, ctx.Ebx);
		DWORD new_esp = ctx.Esp - 4 * sizeof(DWORD);
		esp_push(&ctx.Esp, new_esp);
		esp_push(&ctx.Esp, ctx.Ebp);
		esp_push(&ctx.Esp, ctx.Esi);
		esp_push(&ctx.Esp, ctx.Edi);

		sched_store_context(core_number, ctx);
	}

	ctx.Eip = (DWORD32) entry_point;

	SetThreadContext(target_core, &ctx);
	while (ResumeThread(target_core))
		;
}

DWORD WINAPI core_int_thread_entry(void *param)
{
	int core_number = (int) param;
	int_thread_running[core_number] = true;

	init_cpu_core(core_number);

	while (int_thread_running[core_number]) {
		int num = WaitForMultipleObjects(INTERRUPT_COUNT,
			cpu_int_table_handlers[core_number], false, INFINITE);

		if (num == WAIT_FAILED)
		{
			printf("Interrupt error %d\n", GetLastError());
			continue;
		}

		if (cpu_int_table_masked[core_number][num])
			continue;

		switch (num) {
		case INT_SCHEDULER:
			semaphore_P(sched_lock, 1);
		case INT_RESCHEDULE:
			core_do_interrupt(cpu_int_table_routines[core_number][num], core_number);
			break;
		case INT_CORE_TERM:
			int_thread_running[core_number] = false;
			TerminateThread(core_handles[core_number], 0);
			break;
		case INT_CORE_RESUME:
			while (ResumeThread(core_handles[core_number]))
				;
			break;
		case INT_CORE_SUSPEND:
			SuspendThread(core_handles[core_number]);
			break;
		}

		ResetEvent(cpu_int_table_handlers[core_number][num]);
	}
	return 0;
}
