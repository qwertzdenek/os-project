// console.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <string>

#include "bootstrap.h"
#include "sched_calls.h"
#include "cpu.h"
#include "scheduler.h"

#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

char *help()
{
	return
		"KIV/OS Scheduler project\n"
		"Authors:\n"
		"  Zdenìk Janeèek (ycdmdj@gmail.com)\n"
		"  David Fiedler (david.fido.fiedler@gmail.com)\n"
		"  Tomáš Cígler (drtikozel@gmail.com)\n"
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
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

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
	int number;

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
			exec_task(RUNNER, NULL);
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
		}
		else if (input == "pause-core")
		{
			std::cout << "> ";
			std::cin >> number;

			// call interrupt
			if (number >= 0 && number <= CORE_COUNT)
			{
				if (number == 0)
				{
					std::cout << "Core 0 cannot be paused" << std::endl;
				}
				else 
				{
					if (core_paused[number])
					{
						std::cout << "Core already paused" << std::endl;
					}
					else
					{
						core_paused[number] = true;
						cpu_int_table_messages[number][4];
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
			std::cin >> number;

			// call interrupt
			if (number >= 0 && number <= CORE_COUNT)
			{
				if (core_paused[number])
				{
					core_paused[number] = false;
					cpu_int_table_messages[number][3];
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
