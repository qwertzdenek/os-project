#include "stdafx.h"
#include "bootstrap.h"
#include "cpu.h"
#include "core_int_thread.h"
#include "interrupts.h"
#include "scheduler.h"

HANDLE power_button_event;
HANDLE hw_thread;

HANDLE core_int_handles[CORE_COUNT];

void cpu_init()
{
	memset(cpu_int_table_messages, 0, CORE_COUNT * INTERRUPT_COUNT * sizeof(void *));

	// scheduler interrupt timer
	for (int core = 0; core < CORE_COUNT; core++)
	{
		// schedule
		cpu_int_table_handlers[core][INT_SCHEDULER] = CreateEvent(NULL, TRUE, FALSE, NULL);
		cpu_int_table_masked[core][INT_SCHEDULER] = core == 0 ? false : true;
		cpu_int_table_routines[core][INT_SCHEDULER] = do_schedule;

		// reschedule
		cpu_int_table_handlers[core][INT_RESCHEDULE] = CreateEvent(NULL, TRUE, FALSE, NULL);
		cpu_int_table_masked[core][INT_RESCHEDULE] = false;
		cpu_int_table_routines[core][INT_RESCHEDULE] = do_reschedule;

		// stop
		cpu_int_table_handlers[core][INT_CORE_TERM] = CreateEvent(NULL, TRUE, FALSE, NULL);
		cpu_int_table_masked[core][INT_CORE_TERM] = false;
		cpu_int_table_routines[core][INT_CORE_TERM] = NULL;

		// resume
		cpu_int_table_handlers[core][INT_CORE_RESUME] = CreateEvent(NULL, TRUE, FALSE, NULL);
		cpu_int_table_masked[core][INT_CORE_RESUME] = false;
		cpu_int_table_routines[core][INT_CORE_RESUME] = NULL;

		// suspend
		cpu_int_table_handlers[core][INT_CORE_SUSPEND] = CreateEvent(NULL, TRUE, FALSE, NULL);
		cpu_int_table_masked[core][INT_CORE_SUSPEND] = false;
		cpu_int_table_routines[core][INT_CORE_SUSPEND] = NULL;

		// initialize rest of them as masked
		for (int i = 5; i < INTERRUPT_COUNT; i++)
		{
			cpu_int_table_handlers[core][i] = CreateEvent(NULL, TRUE, FALSE, NULL);
			cpu_int_table_masked[core][i] = true;
			cpu_int_table_routines[core][i] = NULL;
		}

		core_int_handles[core] = CreateThread(NULL, 0, core_int_thread_entry, (void *)core, 0, NULL);
		SetThreadAffinityMask(core_int_handles[core], 0x1 << core);
	}
}

void cpu_stop()
{
	for (int core = 0; core < CORE_COUNT; core++)
	{
		// stop it and wait
		SetEvent(cpu_int_table_handlers[core][INT_CORE_TERM]);
		WaitForSingleObject(core_int_handles[core], INFINITE);

		for (int i = 0; i < INTERRUPT_COUNT; i++)
		{
			CloseHandle(cpu_int_table_handlers[core][i]);
		}
	}
}

DWORD __stdcall hardware_entry(void *)
{
	// create CPU with all cores and int handlers
	cpu_init();
	init_scheduler();

	while (WaitForSingleObject(power_button_event, 500) != WAIT_OBJECT_0)
	{
		SetEvent(cpu_int_table_handlers[0][INT_SCHEDULER]);
	}

	cpu_stop();
	return 0;
}

void hardware_start()
{
	power_button_event = CreateEvent(NULL, true, false, NULL);

	hw_thread = (HANDLE) CreateThread(NULL, 256, hardware_entry, 0, 0, NULL);
	SetThreadPriority(hw_thread, THREAD_PRIORITY_HIGHEST);
}

void power_button()
{
	SetEvent(power_button_event);
	WaitForSingleObject(hw_thread, INFINITE);
}
