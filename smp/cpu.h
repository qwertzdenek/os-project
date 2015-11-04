#pragma once

#include <vector>
#include <memory>

#include "core.h"

namespace SMP {
	class CPU
	{
	public:
		CPU();
		~CPU();

		int count();
		void reschedule(CONTEXT *task, int core);
		void set_scheduler_entry(int(*p)(void *));
	private:
		std::vector<std::shared_ptr<Core>> cores;
	};
};
