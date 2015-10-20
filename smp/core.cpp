#include "stdafx.h"
#include "core.h"

namespace SMP
{
	DWORD WINAPI Core::CPUCoreThread(void * param)
	{
		return ERROR_SUCCESS;
	}

	DWORD WINAPI Core::ClockThreadStart(void * Param)
	{
		Core* This = (Core*)Param;
		return This->ClockThread();
	}

	DWORD Core::ClockThread()
	{
		bool doQuit;
		do {
			doQuit = WaitForSingleObject(QuitFlag, ThreadTimeQuantum) == WAIT_OBJECT_0;
			SuspendThread(CPUCore);

			if (!doQuit && ContextToSwitch != NULL) {
				CONTEXT ActualContextState;
				memset(&ActualContextState, 0, sizeof(ActualContextState));
				GetThreadContext(CPUCore, &ActualContextState);

				// store actual state
				memcpy(LastContext, &ActualContextState, sizeof(CONTEXT));

				SetThreadContext(CPUCore, ContextToSwitch);
				LastContext = ContextToSwitch;
				ContextToSwitch = NULL;
			}

			ResumeThread(CPUCore);
		} while (!doQuit);

		return ERROR_SUCCESS;
	}
	Core::Core()
	{
		Core(0);
	}
	Core::Core(int affinity)
	{
		memset(&DefaultContext, 0, sizeof(DefaultContext));
		CPUCore = (HANDLE)CreateThread(NULL, 0, &CPUCoreThread, 0, CREATE_SUSPENDED, NULL);

		SetThreadAffinityMask(CPUCore, 0x1 << affinity);

		DefaultContext.ContextFlags = CONTEXT_ALL;
		GetThreadContext(CPUCore, &DefaultContext);

		// init state pointers
		LastContext = &DefaultContext;
		ContextToSwitch = NULL;

		QuitFlag = CreateEvent(NULL, TRUE, FALSE, NULL);

		Clock = (HANDLE) CreateThread(NULL, 0, &ClockThreadStart, (void*) this, 0, NULL);
	}
	Core::~Core()
	{
	}
	Core::Core(const Core & c) :
		CPUCore(c.CPUCore),
		Clock(c.Clock),
		QuitFlag(c.QuitFlag),
		DefaultContext(c.DefaultContext),
		ContextToSwitch(c.ContextToSwitch),
		LastContext(c.LastContext)
	{
	}
	void Core::reschedule(CONTEXT *task)
	{
		ContextToSwitch = task;
	}
	void Core::shutdown()
	{
		SetEvent(QuitFlag);
		HANDLE threads[2] = { Clock, CPUCore };

		ResumeThread(CPUCore);

		WaitForMultipleObjects(2, threads, true, INFINITE);

		CloseHandle(QuitFlag);
	}
	DWORD Core::getCPUId()
	{
		return GetThreadId(CPUCore);
	}
};
