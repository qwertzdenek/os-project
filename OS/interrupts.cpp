#include "stdafx.h"
#include "core.h"

#include "scheduler.h"

__declspec(naked) void do_schedule()
{
	// this will run only on the first core
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

		call actual_core
		mov ecx, eax
		mov eax, 4; pointer is 4 bytes
		mov ebx, INTERRUPT_COUNT
		mul ebx
		mov eax, ecx    ; core number
		mul ebx
		lea ebx, cpu_int_table_messages[eax + 4]
		mov esp, dword ptr [ebx]
		mov dword ptr [ebx], 0

		popad
		popfd

		ret
	}
}

__declspec(naked) void do_start()
{
	// ResumeThread()
}

__declspec(naked) void do_stop()
{
	// TerminateThread()
}
