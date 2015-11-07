#include "stdafx.h"
#include "core_int_thread.h"
#include "sched_int_tick.h"

HANDLE quit_flag;

DWORD WINAPI sched_int_tick_entry(void *param)
{
	bool quit = false;

	do
	{
		quit = WaitForSingleObject(quit_flag, 500) == WAIT_OBJECT_0;

		// We will primary schedule on the first core.
		SetEvent(scheduler_interrupt_handle[0]);
	} while (!quit);

	return 0;
}

// it will return quit handle
HANDLE sched_tick_int_init()
{
	HANDLE tick_thread = CreateThread(NULL, 0, sched_int_tick_entry, NULL, 0, NULL);
	SetThreadPriority(tick_thread, THREAD_PRIORITY_HIGHEST);

	quit_flag = CreateEvent(NULL, TRUE, FALSE, NULL);

	return quit_flag;
}
