#include "stdafx.h"
#include "core.h"

#include "scheduler.h"

__declspec(naked) void do_schedule()
{
	// this will be run only on first core
	__asm
	{
		; push flags
		pushfd

		; push general registers
		pushad

		call scheduler_run

		mov esp, eax

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
