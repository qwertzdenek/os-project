#pragma once

#include <queue>

#include "task.h"

#define ThreadStackSize 1024*1024

class Scheduler : public Task {
private:
	static const int NUMBER_OF_TICKS_TO_RESCHEDULE = 100;
	std::queue<Task> processQueue;
	long tickCount = 0;

	CONTEXT default_context;

	bool create_task(std::string name, std::string *arg[]);

public:
	Scheduler();
	~Scheduler();

	int main(int argc, std::string * argv[]) override;
};
