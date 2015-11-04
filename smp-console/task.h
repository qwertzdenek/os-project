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

struct start_param {
	void *task_class;
	void *data;
};

class Task {
private:
	start_param param;
public:
	Task()
	{
		param.data = NULL;
		param.task_class = this;
	}

	explicit
	Task(std::vector<std::string> arg)
	{
		param.data = new std::vector<std::string>(arg);
		param.task_class = this;
	}

	~Task()
	{
		delete param.data;
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
