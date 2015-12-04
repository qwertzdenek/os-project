#include "stdafx.h"

#include <iostream>
#include <string>

#include "bootstrap.h"
#include "sched_calls.h"
#include "cpu.h"
#include "scheduler.h"

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
		"  pause-core - suspends core execution. You cannot stop thread number 0.\n"
		"  resume-core - resumes core code execution\n\n";
}

int main(int argc, char *argv[], char *envp[])
{
	bool help_flag = false;

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

	Sleep(200);

	while (running)
	{
		std::cin >> input;
		std::transform(input.begin(), input.end(), input.begin(), ::tolower);

		if (input == "exit")
		{
			running = false;
		}
		else if (input == "start")
		{
			exec_task(RUNNER, std::make_shared<task_run_parameters>(1, 2));
		}
		else if (input == "help")
		{
			std::cout << help() << std::endl;
		}
		else if (input == "show")
		{
			std::cout << "-> Runnable tasks:" << std::endl;
			std::cout << sched_get_runnable_tasks() << std::endl;
			std::cout << "-> Running tasks:" << std::endl;
			std::cout << sched_get_running_tasks() << std::endl;
			std::cout << "-> Cores:" << std::endl;
			std::cout << sched_get_cores_info() << std::endl;
		}
		else if (input == "pause-core")
		{
			std::cout << "> ";
			std::cin >> core_number;

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
			std::cout << "> ";
			std::cin >> core_number;

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
		else
		{
			std::cout << "Unknown command" << std::endl;
		}
	}

	power_button();

	return 0;
}
