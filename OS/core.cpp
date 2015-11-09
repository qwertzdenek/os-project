#include "stdafx.h"
#include <stdio.h>

#include "core.h"
#include "smp.h"

HANDLE core_handle[SMP::numberOfcores];

DWORD WINAPI init_cpu_idle_task(void *param)
{
	while (true)
		printf("Hello\n");

	return 0;
}

void init_cpu_core(int core_number)
{
	core_handle[core_number] = (HANDLE)CreateThread(NULL, 0, init_cpu_idle_task, 0, 0, NULL);
	SetThreadAffinityMask(core_handle[core_number], 0x1 << core_number);
}

void deinit_cpu_core()
{
	// TODO: it will be called as interrupt
	ExitThread(0);
}
