#pragma once

#include "core.h"

namespace SMP {
	class CPU
	{
	public:
		CPU();
		~CPU();

		int count();
		int reschedule(TThreadControlBlock *thread, int core);
	private:
		Core *cores;
		unsigned int core_count;
	};
};
