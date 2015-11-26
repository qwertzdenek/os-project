// console.cpp : Defines the entry point for the console application.
//

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
		"  Zden�k Jane�ek (ycdmdj@gmail.com)\n"
		"  David Fiedler (david.fido.fiedler@gmail.com)\n"
		"  Tom� C�gler (drtikozel@gmail.com)\n"
		"Usage:\n"
		"  -h  show this message\n";
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
	int number;

	// boot up
	hardware_start();

	/*Sleep(200);

	exec_task(RUNNER, NULL);
	exec_task(RUNNER, NULL);
	exec_task(RUNNER, NULL);
	exec_task(RUNNER, NULL);
	exec_task(RUNNER, NULL);
	exec_task(RUNNER, NULL);
	exec_task(RUNNER, NULL);
	exec_task(RUNNER, NULL);*/

	while (running)
	{
		std::cin >> input;
		std::transform(input.begin(), input.end(), input.begin(), ::tolower);

		if (input == "close")
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
			std::cout << "Task in front:" << std::endl;
			std::cout << "id state type" << std::endl;
			for (int i = 0; i < task_queue.size(); i++)
			{
				std::cout << task_queue[i]->task_id << " " << task_queue[i]->state << "" << task_queue[i]->type << std::endl;
			}
			std::cout << std::endl;
			std::cout << "Running tasks:" << std::endl;
			std::cout << "id state type" << std::endl;
			for (int i = 0; i < CORE_COUNT; i++)
			{
				if (running_tasks[i] != NULL)
				{
					std::cout << running_tasks[i]->task_id << " " << running_tasks[i]->state << "" << running_tasks[i]->type << std::endl;
				}
			}
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
