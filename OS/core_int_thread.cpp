#include "stdafx.h"
#include "common.h"
#include "core.h"
#include "core_int_thread.h"
#include "smp.h"

HANDLE scheduler_interrupt_handle[SMP::NUMBER_OF_CORES];
HANDLE start_interrupt_handle[SMP::NUMBER_OF_CORES];
HANDLE stop_interrupt_handle[SMP::NUMBER_OF_CORES];

HANDLE core_tick[SMP::NUMBER_OF_CORES];

task_control_block core_tasks[SMP::NUMBER_OF_CORES];

// TODO: find universal alternative (works only on x86)
__declspec(naked) void scheduler_interrupt()
{
	__asm
	{
		// push status word 
		pushfd

		// push all registers  
		pushad
	}

	// reschedule 
	// TODO: run scheduler
	//reschedule();
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
		ctx.Rsp = (DWORD64) ctx.Rip;

		core_tasks[core_number].stack = (void *)ctx.Rsp;

		ctx.Rip = (DWORD64) scheduler_interrupt;
	#else
		ctx.Esp -= sizeof(DWORD32);
		ctx.Esp = (DWORD32) ctx.Eip;

		core_tasks[core_number].stack = (void *) ctx.Esp;

		ctx.Eip = (DWORD32)scheduler_interrupt;
	#endif

	SetThreadContext(target_core, &ctx);
	ResumeThread(target_core);
}

void core_int_handle(int core_number)
{
	core_do_interrupt(core_number);
}

DWORD WINAPI core_int_thread_entry(void *param)
{
	int core_number = *(int *) param;
	bool running = true;

	HANDLE interrupts[] = { scheduler_interrupt_handle[core_number], stop_interrupt_handle[core_number] };
	while (running) {
		int msg = WaitForMultipleObjects(2, interrupts, false, INFINITE) == WAIT_OBJECT_0;

		switch (msg)
		{
		case 0:
			core_int_handle(core_number);
			ResetEvent(scheduler_interrupt_handle[core_number]);
			break;
		case 1:
			// TODO: stop core thread
			ResetEvent(stop_interrupt_handle[core_number]);
			break;
		default:
			break;
		}
	}
	return 0;
}

void core_int_init(int core_number)
{
	scheduler_interrupt_handle[core_number] = CreateEvent(NULL, TRUE, FALSE, NULL);
	start_interrupt_handle[core_number] = CreateEvent(NULL, TRUE, FALSE, NULL);
	stop_interrupt_handle[core_number] = CreateEvent(NULL, TRUE, FALSE, NULL);

	CreateThread(NULL, 0, core_int_thread_entry, &core_number, 0, NULL);
}
