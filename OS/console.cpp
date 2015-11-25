// console.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <string>

#include "bootstrap.h"
#include "sched_calls.h"
#include "cpu.h"

char *help()
{
	return
		"KIV/OS Scheduler project\n"
		"Authors:\n"
		"  Zdenìk Janeèek (ycdmdj@gmail.com)\n"
		"  David Fiedler (david.fido.fiedler@gmail.com)\n"
		"  Tomáš Cígler (drtikozel@gmail.com)\n"
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
		else if (input == "pause-core")
		{
			std::cout << "> ";
			std::cin >> number;

			// call interrupt
			cpu_int_table_messages[number][4];
		}
		else if (input == "resume-thread")
		{
			std::cout << "> ";
			std::cin >> number;

			// call interrupt
			cpu_int_table_messages[number][3];
		}
		else
		{
			std::cout << "Unknown command" << std::endl;
		}
	}

	power_button();

	return 0;
}
