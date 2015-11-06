#include <Windows.h>

#include "core.h"
#include "core_int_thread.h"

HANDLE scheduler_interrupt_handle[CORE_COUNT];
HANDLE start_interrupt_handle[CORE_COUNT];
HANDLE stop_interrupt_handle[CORE_COUNT];

__declspec(naked) void scheduler_interrupt()
{
	__asm
	{
		// push status word 
		pushfd

		// push all registers  
		pushad

		// TODO: get actual TCB and store stack here
		// push stack ptr to running thread
		mov edx, wcs_running_thread_tcb
		mov[edx + WCS_THREAD_TCB_STACK_PTR_OFFSET], esp
	}

	// reschedule 
	reschedule();
}

void core_do_interrupt(int core_number)
{
	CONTEXT ctx;
	memset(&ctx, 0, sizeof(ctx));
	ctx.ContextFlags = CONTEXT_FULL;

	HANDLE target_core = core_handle[core_number];

	SuspendThread(target_core);
	GetThreadContext(target_core, &ctx);

	// push return address on task stack
	// set interrupt handler on the cpu core
	#ifdef _M_X64
		ctx.Rsp -= sizeof(DWORD64);
		ctx.Rsp = (DWORD64) ctx.Eip;

		ctx.Rip = (DWORD64) scheduler_interrupt;
	#else
		ctx.Esp -= sizeof(DWORD32);
		ctx.Esp = (DWORD32) ctx.Eip;

		ctx.Eip = (DWORD32)scheduler_interrupt;
	#endif

	SetThreadContext(target_core, &ctx);
	ResumeThread(target_core);
}

void core_handle_int(int core_number)
{
	core_do_interrupt(core_number);
}

void core_int_thread_entry(int core_number)
{
	bool running = true;

	HANDLE interrupts[] = { scheduler_interrupt_handle[core_number], stop_interrupt_handle[core_number] };
	while (running) {
		int msg = WaitForMultipleObjects(2, interrupts, false, INFINITE) == WAIT_OBJECT_0;

		switch (msg)
		{
		case 0:
			core_handle_int(core_number);
			break;
		case 1:
			
			break;
		default:
			break;
		}
	}
}

