// console.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <string>

#include "console.h"
#include "core.h"
#include "core_int_thread.h"
#include "sched_int_tick.h"
#include "scheduler.h"

bool running;

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

HANDLE get_main_thread_handle()
{
	HANDLE main_thread_handle = 0;

	// get it
	DuplicateHandle(GetCurrentProcess(),
		GetCurrentThread(),
		GetCurrentProcess(),
		&main_thread_handle,
		0,
		TRUE,
		DUPLICATE_SAME_ACCESS);

	// return it
	return main_thread_handle;
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

	while (true)
	{
		
		std::cin >> input;
		
		if(input == "start")
		{
			start();
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

	

	return 0;
}

void start()
{
	if (running)
	{
		std::cout << "SMP already running";
	}
	else 
	{
		DWORD thread_id;
		HANDLE main_handle = (HANDLE)CreateThread(NULL, TASK_STACK_SIZE, start_smp, 0, 0, &thread_id);
	}
	
}

DWORD WINAPI start_smp(void *param)
{
	init_cpu_core(0);
	init_cpu_core(1);
	init_cpu_core(2);
	init_cpu_core(3);
	init_cpu_int_table();
	init_scheduler();
	sched_int_tick(get_main_thread_handle());

	return 0;
}
