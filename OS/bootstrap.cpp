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

		core_int_handles[core] = CreateThread(NULL, 0, core_int_thread_entry, (void *)core, 0, NULL);
		SetThreadAffinityMask(core_int_handles[core], 0x1 << core);
	}
}

void cpu_stop()
{
	for (int core = 0; core < CORE_COUNT; core++)
	{
		// stop it and wait
		SetEvent(cpu_int_table_handlers[core][2]);
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
		SetEvent(cpu_int_table_handlers[0][0]);
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
