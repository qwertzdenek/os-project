#include "stdafx.h"
#include "cpu.h"
#include "sched_int_tick.h"

bool quit = false;

// it will return quit handle
int sched_int_tick(const HANDLE main_thread)
{
	SetThreadPriority(main_thread, THREAD_PRIORITY_HIGHEST);
	do
	{
		Sleep(500);

		SetEvent(cpu_int_table_handlers[0][0]);
	} while (!quit);

	return 0;
}
