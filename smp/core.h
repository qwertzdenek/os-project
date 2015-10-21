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
		CONTEXT *ContextToSwitch;
		CONTEXT *LastContext;

		int number;

		static DWORD WINAPI Core::CPUCoreThread(void * param);
		static DWORD WINAPI ClockThreadStart(void* Param);
		DWORD ClockThread();
	public:
		Core();
		Core(int affinity);
		~Core();
		Core(const Core& c);

		void reschedule(CONTEXT *task);
		DWORD getCPUId();
	};
};
