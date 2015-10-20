#include "stdafx.h"
#include "cpu.h"

#include <thread>

namespace SMP {
	CPU::CPU()
	{
		core_count = std::thread::hardware_concurrency();
	}
	CPU::~CPU()
	{
	}
	int CPU::count()
	{
		return core_count;
	}
	int CPU::reschedule(TThreadControlBlock *thread, int core)
	{
		cores[core].reschedule(thread);
		return ERROR_SUCCESS;
	}
};
