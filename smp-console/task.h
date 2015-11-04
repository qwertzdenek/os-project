#pragma once

#include <vector>
#include <string>
#include <vector>

#include <process.h>
#include <Windows.h>

#include "sched_calls.h"

typedef enum {
	NEW, RUNNABLE, BLOCKED, RUNNING, TERMINATED
} ETaskState;

typedef struct {
	CONTEXT context;
	HANDLE stack_handle;
	void* stack;
	int task_id;
	time_t started;
	ETaskState state;
} TTaskControlBlock;


class Task {
private:
	std::vector<std::string> arg;

public:
	Task() {}

	explicit
	Task(std::vector<std::string> arg)
	{
		this->arg.insert(this->arg.end(), arg.begin(), arg.end());
	}

	Task(const Task &o)
	{
		arg.clear();
		arg.insert(arg.end(), o.arg.begin(), o.arg.end());
	}

	Task(Task&& o) noexcept : arg(std::move(arg)) {}

	Task& operator=(Task&& o)
	{
		arg = std::move(o.arg);
		return *this;
	}

	// task entry point
	unsigned WINAPI start(void * param) {
		// load parameters
		std::vector<std::string> arg = *static_cast<std::vector<std::string> *>(param);

		// call main
		int code = main(arg);

		// terminate process
		exit_task(code);
	}

	// implement this
	virtual int main(std::vector<std::string> arg);
};
