#include "stdafx.h"
#include "cpu.h"

#include <thread>
#include <iostream>

namespace SMP {
	CPU::CPU()
	{
		int count = std::thread::hardware_concurrency();

		for (int i = 0; i < count; i++)
		{
			cores.push_back(Core(i));
			std::cout << "ID: " << cores[i].getCPUId() << std::endl;
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
		cores[core].reschedule(task);
	}
};
