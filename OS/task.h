#pragma once

#include <list>
#include <string>

#include <process.h>
#include <Windows.h>

#include "common.h"
#include "sched_calls.h"

class Task {
private:
	std::string *argv;

public:
	Task() {}

	explicit Task(std::string *argv) : argv(argv) {}

	Task(const Task &o) : argv(o.argv) {}

	Task(Task&& o) noexcept : argv(std::move(argv)) {}

	Task& operator=(Task&& o)
	{
		argv = std::move(o.argv);
		return *this;
	}

	// task entry point
	unsigned WINAPI start(void * param) {
		// load parameters
		std::string *argv = static_cast<std::string *>(param);

		int argc = 0;

		// TODO: load list of arguments

		// call main
		int code = main(argc, &argv);

		// terminate process
		exit_task(code);
	}

	// implement this
	virtual int main(int argc, std::string *argv[]);
};
