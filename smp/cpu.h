#pragma once

#include <vector>

#include "core.h"

namespace SMP {
	class CPU
	{
	public:
		CPU();
		~CPU();

		int count();
		void reschedule(CONTEXT *task, int core);
	private:
		std::vector<Core> cores;
	};
};
