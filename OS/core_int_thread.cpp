#include "stdafx.h"
#include "core.h"
#include "core_int_thread.h"

#include "interrupts.h"

// interrupt table
bool cpu_int_table_masked[CORE_COUNT][INTERRUPT_COUNT];
void *cpu_int_table_routines[CORE_COUNT][INTERRUPT_COUNT];
HANDLE cpu_int_table_handlers[CORE_COUNT][INTERRUPT_COUNT];
void *cpu_int_table_messages[CORE_COUNT][INTERRUPT_COUNT];

// controls int threads
bool int_thread_running[CORE_COUNT];

void core_do_stop(int core_number)
{
	int_thread_running[core_number] = false;
	TerminateThread(core_handle[core_number], 0);
}

void core_do_resume(int core_number)
{
	ResumeThread(core_handle[core_number]);
}

void core_do_suspend(int core_number)
{
	SuspendThread(core_handle[core_number]);
}

void core_do_interrupt(void *entry_point, int core_number)
{
	CONTEXT ctx;
	memset(&ctx, 0, sizeof(ctx));
	ctx.ContextFlags = CONTEXT_FULL;

	HANDLE target_core = core_handle[core_number];

	SuspendThread(target_core);
	GetThreadContext(target_core, &ctx);
	
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

	ctx.Eip = (DWORD32) entry_point;

	SetThreadContext(target_core, &ctx);
	ResumeThread(target_core);
}

DWORD WINAPI core_int_thread_entry(void *param)
{
	int core_number = (int) param;
	int_thread_running[core_number] = true;

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
		case 0:
		case 1:
			core_do_interrupt(cpu_int_table_routines[core_number][num], core_number);
			break;
		case 2:
			core_do_stop(core_number);
			break;
		case 3:
			core_do_resume(core_number);
			break;
		case 4:
			core_do_suspend(core_number);
			break;
		}

		ResetEvent(cpu_int_table_handlers[core_number][num]);
	}
	return 0;
}

void init_cpu_int_table()
{
	memset(cpu_int_table_messages, 0, CORE_COUNT * INTERRUPT_COUNT * sizeof(void *));

	// scheduler interrupt timer
	for (int core = 0; core < CORE_COUNT; core++)
	{
		// schedule
		cpu_int_table_handlers[core][0] = CreateEvent(NULL, TRUE, FALSE, NULL);
		cpu_int_table_masked[core][0] = core == 0 ? false : true;
		cpu_int_table_routines[core][0] = do_schedule;

		// reschedule
		cpu_int_table_handlers[core][1] = CreateEvent(NULL, TRUE, FALSE, NULL);
		cpu_int_table_masked[core][1] = false;
		cpu_int_table_routines[core][1] = do_reschedule;

		// stop
		cpu_int_table_handlers[core][2] = CreateEvent(NULL, TRUE, FALSE, NULL);
		cpu_int_table_masked[core][2] = false;
		cpu_int_table_routines[core][2] = NULL;

		// resume
		cpu_int_table_handlers[core][3] = CreateEvent(NULL, TRUE, FALSE, NULL);
		cpu_int_table_masked[core][3] = false;
		cpu_int_table_routines[core][3] = NULL;

		// suspend
		cpu_int_table_handlers[core][4] = CreateEvent(NULL, TRUE, FALSE, NULL);
		cpu_int_table_masked[core][4] = false;
		cpu_int_table_routines[core][4] = NULL;

		// initialize rest of them as masked
		for (int i = 5; i < INTERRUPT_COUNT; i++)
		{
			cpu_int_table_handlers[core][i] = CreateEvent(NULL, TRUE, FALSE, NULL);
			cpu_int_table_masked[core][i] = true;
			cpu_int_table_routines[core][i] = NULL;
		}

		HANDLE int_thread = CreateThread(NULL, 0, core_int_thread_entry, (void *) core, 0, NULL);
		SetThreadAffinityMask(int_thread, 0x1 << core);
	}
}
