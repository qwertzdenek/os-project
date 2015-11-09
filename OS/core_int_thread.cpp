#include "stdafx.h"
#include "common.h"
#include "core.h"
#include "core_int_thread.h"

#include "interrupts.h"

HANDLE core_tick[CORE_COUNT];

CPU_INT interrupt_table[CORE_COUNT][INTERRUPT_COUNT];

// actual tasks on cpu
task_control_block *core_tasks[CORE_COUNT];

void core_do_interrupt(void *entry_point, int core_number)
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

		core_tasks[core_number]->stack = (void *)ctx.Rsp;

		ctx.Rip = (DWORD64) entry_point;
	#else
		ctx.Esp -= sizeof(DWORD32);
		ctx.Esp = (DWORD32) ctx.Eip;

		core_tasks[core_number]->stack = (void *) ctx.Esp;

		ctx.Eip = (DWORD32) entry_point;
	#endif

	SetThreadContext(target_core, &ctx);
	ResumeThread(target_core);
}

DWORD WINAPI core_int_thread_entry(void *param)
{
	int core_number = *(int *) param;
	bool running = true;
	int msg = 0;

	// TODO: pøizpùsobit na interrupt_table[core][1]
	// zjistit èíslo vyjímky

	while (running) {
//		int msg = WaitForMultipleObjects(interrupt_table_count[core_number],
//			interrupt_table[core_number], false, INFINITE) == WAIT_OBJECT_0;

		switch (msg)
		{
		case 0:
			// TODO: stop core thread
			//ResetEvent(stop_interrupt_handle[core_number]);
			break;
		case 5:
			core_do_interrupt(do_schedule, core_number);
			//ResetEvent(scheduler_interrupt_handle);
			break;
		default:
			break;
		}
	}
	return 0;
}

void core_int_init()
{
	// scheduler interrupt timer
	interrupt_table[0][0].handler = CreateEvent(NULL, TRUE, FALSE, NULL);
	interrupt_table[0][0].is_masked = false;
	interrupt_table[0][0].routine = do_schedule;

	for (int core = 1; core < CORE_COUNT; core++)
	{
		// reschedule
		interrupt_table[core][1].handler = CreateEvent(NULL, TRUE, FALSE, NULL);
		interrupt_table[core][1].is_masked = false;
		interrupt_table[core][1].routine = do_reschedule;

		// start
		interrupt_table[core][2].handler = CreateEvent(NULL, TRUE, FALSE, NULL);
		interrupt_table[core][2].is_masked = false;
		interrupt_table[core][3].routine = do_start;

		//stop
		interrupt_table[core][3].handler = CreateEvent(NULL, TRUE, FALSE, NULL);
		interrupt_table[core][3].is_masked = false;
		interrupt_table[core][3].routine = do_stop;

		CreateThread(NULL, 0, core_int_thread_entry, &core, 0, NULL);
	}
	
}
