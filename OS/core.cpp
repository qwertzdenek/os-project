#include "stdafx.h"
#include <stdio.h>

#include "core.h"
#include "tasks.h"

HANDLE core_handles[CORE_COUNT];
DWORD core_thread_id[CORE_COUNT];

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

void init_cpu_core(int core_number)
{
	DWORD thread_id;
	core_handles[core_number] = (HANDLE)CreateThread(NULL, 0, NULL, 0, CREATE_SUSPENDED, &thread_id);
	SetThreadAffinityMask(core_handles[core_number], 0x1 << core_number);

	core_thread_id[core_number] = thread_id;
	core_paused[core_number] = true;
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
