#pragma once

#include "task.h"

#define ThreadStackSize 1024*1024

class Scheduler : Task {
private:
	static const int NUMBER_OF_TICKS_TO_RESCHEDULE = 100;
	std::queue<Task> processQueue;
	long tickCount = 0;

	CONTEXT default_context;

	bool create_task(std::string name);

public:
	Scheduler();
	~Scheduler();

	int main(std::vector<std::string> arg) override;
};
