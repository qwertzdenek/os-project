#pragma once

#include <queue>
#include "process_descriptor.h"
#include "process.h"


namespace SMP {
	class Scheduler : Process
	{
	public:
		Scheduler();
		~Scheduler();
		void run();
	private:
		static const int NUMBER_OF_TICKS_TO_RESCHEDULE = 100;
		std::queue<ProcessDescriptor> processQueue;
		int tickCount = 0;
	};
}
