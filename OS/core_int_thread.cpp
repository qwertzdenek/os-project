#include "stdafx.h"

#include "core_int_thread.h"
#include "scheduler.h"
#include "core.h"
#include "sched_calls.h"

// controls int threads
bool int_thread_running[CORE_COUNT];

// Old context is stored by scheduler,
// we will do only rescheduling
void core_do_reschedule(int core_number)
{
	CONTEXT ctx;

	memset(&ctx, 0, sizeof(ctx));
	ctx.ContextFlags = CONTEXT_FULL;

	SuspendThread(core_handles[core_number]);
	GetThreadContext(core_handles[core_number], &ctx);

	ctx.Ecx = core_number;
	ctx.Eip = (DWORD)cpu_int_table_routines[0][INT_RESCHEDULE];

	semaphore_P(sched_lock, 1);

	SetThreadContext(core_handles[core_number], &ctx);
	while (ResumeThread(core_handles[core_number]))
		;
}

void core_do_schedule()
{
	CONTEXT ctx;

	semaphore_P(sched_lock, 1);

	for (int core = 0; core < CORE_COUNT; core++)
	{
		memset(&ctx, 0, sizeof(ctx));
		ctx.ContextFlags = CONTEXT_FULL;
		HANDLE core_handle = core_handles[core];

		if (sched_active_task(core))
		{
			SuspendThread(core_handle);
			GetThreadContext(core_handle, &ctx);

			// push return address on task stack
			// set interrupt handler on the cpu core
			esp_push(&ctx.Esp, ctx.Eip);

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

			sched_store_context(core, ctx);
		}
	}

	memset(&ctx, 0, sizeof(ctx));
	ctx.ContextFlags = CONTEXT_FULL;
	GetThreadContext(core_handles[0], &ctx);

	ctx.Eip = (DWORD) cpu_int_table_routines[0][INT_SCHEDULER];

	SetThreadContext(core_handles[0], &ctx);
	while (ResumeThread(core_handles[0]))
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
			core_do_schedule();
			break;
		case INT_RESCHEDULE:
			core_do_reschedule(core_number);
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
