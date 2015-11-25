// console.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <string>

#include "bootstrap.h"

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

	std::string input;

	// boot up
	hardware_start();

	while (true)
	{
		
		std::cin >> input;
		
		if(input == "start")
		{
			// TODO
		}
		else if(input == "help")
		{
			std::cout << help() << std::endl;
		}
		else if (input == "stop")
		{
			//TODO
		}
		else if (input.find("pause-thread") == 0)
		{
			//TODO
		}
		else if (input.find("resume-thread") == 0)
		{
			//TODO
		}
		else		
		{
			std::cout << "Unknown command" << std::endl;
		}

		//std::cout << input;
	}

	power_button();

	return 0;
}
