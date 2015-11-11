#pragma once

#include <queue>

#include "task.h"
#include "smp.h"

#define ThreadStackSize 1024*1024

class Scheduler : public Task {
private:
	static const int NUMBER_OF_TICKS_TO_RESCHEDULE = 100;

	static const int TIME_QUANTUM = 25;
	static const int TIME_QUANTUM_DECREASE = 25;

	struct TaskDescriptor {
		Task &task;
		int timeQuantum;
		TaskDescriptor(Task &task, int timeQuantum);
	};

	/// <summary>	Queue of new tasks. </summary>
	std::queue<Task> newTaskQueue;

	/// <summary>	Main task queue. </summary>
	std::queue<TaskDescriptor> taskQueue;

	TaskDescriptor* runningTasks[SMP::NUMBER_OF_CORES];

	long tickCount = 0;

	CONTEXT default_context;

	bool create_task(std::string name, std::string *arg[]);

	void processNewTasks();
	void processQueue();
	void runTask(TaskDescriptor &taskDescriptor);

public:
	Scheduler();
	~Scheduler();

	void run();
};
