#include "stdafx.h"

#include <iostream>
#include <string>
#include <sstream>

#include "bootstrap.h"
#include "sched_calls.h"
#include "cpu.h"
#include "scheduler.h"

HANDLE error_event;

char *help()
{
	return
		"KIV/OS Scheduler project\n"
		"Authors:\n"
		"  Zdenek Janecek (ycdmdj@gmail.com)\n"
		"  David Fiedler (david.fido.fiedler@gmail.com)\n"
		"  Tomas Cigler (drtikozel@gmail.com)\n"
		"Usage:\n"
		"  -h  show this message\n"
		"Interactive:\n"
		"  exit - close the simulation\n"
		"  start - start producent and consument\n"
		"  help - show this message\n"
		"  show - shows the scheduler status\n"
		"  state - shows task computation progress\n"
		"  pause-core - suspends core execution. You cannot stop thread number 0.\n"
		"  resume-core - resumes core code execution\n\n";
}

void read_stream(int &out, std::string question)
{
	while (true)
	{
		std::cout << question << "> ";
		std::cin >> out;

		if (std::cin.fail())
		{
			std::cin.clear();
			std::cin.ignore(256, '\n');
		}
		else
		{
			break;
		}
	}
	std::cin.ignore(256, '\n'); // ignore rest
}

void read_stream(double &out, std::string question)
{
	while (true)
	{
		std::cout << question << "> ";
		std::cin >> out;

		if (std::cin.fail())
		{
			std::cin.clear();
			std::cin.ignore(256, '\n');
		}
		else
		{
			break;
		}
	}
	std::cin.ignore(256, '\n'); // ignore rest
}

int main(int argc, char *argv[], char *envp[])
{
	bool help_flag = false;
	error_event = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (error_event == NULL)
		return -1;

	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-h"))
		{
			help_flag = true;
		}
	}

	if (help_flag)
	{
		std::cout << help() << std::endl;
		return 0;
	}

	std::cout << "KIV/OS Scheduler project" << std::endl;

	bool running = true;

	std::string input;
	int core_number;

	exec_task(RUNNER, std::make_shared<task_run_parameters>(1, 2));
	exec_task(RUNNER, std::make_shared<task_run_parameters>(1, 3));
	exec_task(RUNNER, std::make_shared<task_run_parameters>(2, 2));
	exec_task(RUNNER, std::make_shared<task_run_parameters>(0, 2));
	exec_task(RUNNER, std::make_shared<task_run_parameters>(1, 1));
	exec_task(RUNNER, std::make_shared<task_run_parameters>(5, 2));
	exec_task(RUNNER, std::make_shared<task_run_parameters>(4, 1));
	exec_task(RUNNER, std::make_shared<task_run_parameters>(3, 2));

	// boot up
	hardware_start();

	while (running && WaitForSingleObject(error_event, 0) != WAIT_OBJECT_0)
	{
		std::cin >> input;
		std::transform(input.begin(), input.end(), input.begin(), ::tolower);

		if (input == "exit")
		{
			running = false;
		}
		else if (input == "start")
		{
			double mean;
			double deviation;

			read_stream(mean, "mean");
			read_stream(deviation, "deviation");

			exec_task(RUNNER, std::make_shared<task_run_parameters>(mean, deviation));
		}
		else if (input == "help")
		{
			std::cout << help() << std::endl;
		}
		else if (input == "show")
		{
			std::stringstream ss;

			ss << "-> Runnable tasks:\n";
			ss << sched_get_runnable_tasks() << '\n';
			ss << "-> Running tasks:\n";
			ss << sched_get_running_tasks() << '\n';
			ss << "-> Cores:\n";
			ss << sched_get_cores_info() << std::endl;

			sched_stream_safe(ss.str());
			ss.clear();
		}
		else if (input == "pause-core")
		{
			read_stream(core_number, "core");

			// call interrupt
			if (core_number >= 0 && core_number < CORE_COUNT)
			{
				if (core_number == 0)
				{
					std::cout << "Core 0 cannot be paused" << std::endl;
				}
				else 
				{
					if (sched_request_pause(core_number))
					{
						std::cout << "Core already paused" << std::endl;
					}
					else
					{
						std::cout << "Core paused" << std::endl;
					}
				}
			}
			else
			{
				std::cout << "Wrong core number" << std::endl;
			}
		}
		else if (input == "resume-core")
		{
			read_stream(core_number, "core");

			// call interrupt
			if (core_number >= 0 && core_number < CORE_COUNT)
			{
				if (sched_request_resume(core_number))
				{
					std::cout << "Core resumed" << std::endl;
				}
				else
				{
					std::cout << "Core is not paused, cannot be resumed" << std::endl;
				}
			}
			else
			{
				std::cout << "Wrong core number" << std::endl;
			}
		}
		else if (input == "state")
		{
			sched_stream_safe(sched_get_tasks_progress());
		}
		else
		{
			std::cout << "Unknown command" << std::endl;
		}
	}

	power_button();

	std::cout << "bye." << std::endl;

	return 0;
}
