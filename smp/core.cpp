#include "stdafx.h"
#include "core.h"

namespace SMP
{
	DWORD WINAPI CPUCoreThread(void * param)
	{
		return ERROR_SUCCESS;
	}

	DWORD Core::ClockThread(void * param)
	{
		bool doQuit;
		do {
			doQuit = WaitForSingleObject(QuitFlag, ThreadTimeQuantum) == WAIT_OBJECT_0;
			SuspendThread(CPUCore);

			if (!doQuit) {
				CONTEXT actualContext;
				memset(&actualContext, 0, sizeof(actualContext));
				GetThreadContext(CPUCore, &actualContext);


				// SetThreadContext(CPUCore, &desiredContext);
			}

			ResumeThread(CPUCore);
		} while (!doQuit);

		return ERROR_SUCCESS;
	}
	Core::Core()
	{
		memset(&DefaultContext, 0, sizeof(DefaultContext));
		CPUCore = (HANDLE)CreateThread(NULL, 0, &CPUCoreThread, 0, CREATE_SUSPENDED, NULL);

		DefaultContext.ContextFlags = CONTEXT_ALL;
		GetThreadContext(CPUCore, &DefaultContext);

		QuitFlag = CreateEvent(NULL, TRUE, FALSE, NULL);
	}
	Core::~Core()
	{
	}
	int Core::reschedule(TThreadControlBlock *thread)
	{
		threadToSwitch = thread;
		return ERROR_SUCCESS;
	}
};
