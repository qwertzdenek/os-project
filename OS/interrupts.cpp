#include "stdafx.h"
#include "core.h"

__declspec(naked) void do_schedule()
{
	// this will be run only on first core
	__asm
	{
		; push flags
		pushfd

		; push general registers
		pushad
	}

	// TODO: store old task esp into his TCB

	// TODO: reschedule - change core_tasks pointer
	// if you want to reschedule another core, store address of
	// tcb on messages table and interrupt it
	// -> SetEvent(cpu_int_table_handlers[core][1])

	__asm
	{
		; pop flags
		popad

		; pop general registers
		popfd

		ret
	}
}

__declspec(naked) void do_reschedule()
{
	__asm
	{
		pushfd
		pushad
	}

	// read message where is target eip

}

__declspec(naked) void do_start()
{
	// ResumeThread()
}

__declspec(naked) void do_stop()
{
	// TerminateThread()
}
