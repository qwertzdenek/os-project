#pragma once

#include "common.h"

namespace SMP
{
	class Core
	{
	private:
		HANDLE CPUCore;
		HANDLE Clock;
		HANDLE QuitFlag;

		CONTEXT DefaultContext;

		TThreadControlBlock *threadToSwitch;

		DWORD WINAPI ClockThread(void* param);
	public:
		Core();
		~Core();

		int reschedule(TThreadControlBlock *thread);
	};
};
