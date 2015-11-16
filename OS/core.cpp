#include "stdafx.h"
#include <stdio.h>

#include "core.h"
#include "tasks.h"

HANDLE core_handle[CORE_COUNT];
DWORD  core_thread_id[CORE_COUNT];

int actual_core()
{
	DWORD current_id = GetCurrentThreadId();
	for (int core = 0; core < CORE_COUNT; core++)
	{
		if (current_id == core_thread_id[core])
			return core;
	}
	return 0;
}

DWORD WINAPI init_cpu_idle_task(void *param)
{
	while (true)
		;

	return 0;
}

void init_cpu_core(int core_number)
{
	DWORD thread_id;
	core_handle[core_number] = (HANDLE)CreateThread(NULL, TASK_STACK_SIZE, init_cpu_idle_task, 0, 0, &thread_id);
	SetThreadAffinityMask(core_handle[core_number], 0x1 << core_number);

	core_thread_id[core_number] = thread_id;
}

void esp_push(DWORD *esp, DWORD value)
{
	*esp -= sizeof(DWORD);
	*(DWORD *)*esp = value;
}

DWORD esp_pop(DWORD *esp)
{
	DWORD value = *(DWORD *)*esp;
	*esp += sizeof(DWORD);
	return value;
}
