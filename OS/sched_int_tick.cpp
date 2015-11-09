#include "stdafx.h"
#include "cpu.h"
#include "sched_int_tick.h"

HANDLE quit_flag;

DWORD WINAPI sched_int_tick_entry(void *param)
{
	bool quit = false;

	do
	{
		quit = WaitForSingleObject(quit_flag, 500) == WAIT_OBJECT_0;

		SetEvent(scheduler_interrupt_handle);
	} while (!quit);

	return 0;
}

// it will return quit handle
HANDLE sched_int_tick_init()
{
	HANDLE tick_thread = CreateThread(NULL, 0, sched_int_tick_entry, NULL, 0, NULL);
	SetThreadPriority(tick_thread, THREAD_PRIORITY_HIGHEST);

	quit_flag = CreateEvent(NULL, TRUE, FALSE, NULL);

	return quit_flag;
}
