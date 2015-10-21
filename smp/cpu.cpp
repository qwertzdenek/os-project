#include "stdafx.h"
#include "cpu.h"

#include <thread>

namespace SMP {
	CPU::CPU()
	{
		int count = std::thread::hardware_concurrency();

		for (int i = 0; i < count; i++)
		{
			std::shared_ptr<Core> core(new Core(i));
			cores.push_back(core);
		}
	}
	CPU::~CPU()
	{
	}
	int CPU::count()
	{
		return cores.size();
	}
	void CPU::reschedule(CONTEXT *task, int core)
	{
		cores[core]->reschedule(task);
	}
};
