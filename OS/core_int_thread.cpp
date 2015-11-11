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
			cpu_int_table_handlers[core_number], false, INFINITE) == WAIT_OBJECT_0;

		if (cpu_int_table_masked[core_number][num])
			continue;

		core_do_interrupt(cpu_int_table_routines[core_number][num], core_number);
		ResetEvent(cpu_int_table_handlers[core_number][num]);
	}
	return 0;
}

void init_cpu_int_table()
{
	memset(cpu_int_table_messages, 0, CORE_COUNT * INTERRUPT_COUNT * sizeof(void *));

	// scheduler interrupt timer
	cpu_int_table_handlers[0][0] = CreateEvent(NULL, TRUE, FALSE, NULL);
	cpu_int_table_masked[0][0] = false;
	cpu_int_table_routines[0][0] = do_schedule;

	for (int core = 0; core < CORE_COUNT; core++)
	{
		// reschedule
		cpu_int_table_handlers[core][1] = CreateEvent(NULL, TRUE, FALSE, NULL);
		cpu_int_table_masked[core][1] = false;
		cpu_int_table_routines[core][1] = do_reschedule;

		// start
		cpu_int_table_handlers[core][2] = CreateEvent(NULL, TRUE, FALSE, NULL);
		cpu_int_table_masked[core][2] = false;
		cpu_int_table_routines[core][2] = do_start;

		//stop
		cpu_int_table_handlers[core][3] = CreateEvent(NULL, TRUE, FALSE, NULL);
		cpu_int_table_masked[core][3] = false;
		cpu_int_table_routines[core][3] = do_stop;

		HANDLE int_thread = CreateThread(NULL, 0, core_int_thread_entry, (void *) core, 0, NULL);
		SetThreadAffinityMask(int_thread, 0x1 << core);
	}
}
