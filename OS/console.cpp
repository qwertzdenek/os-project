// console.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <string.h>

#include "core.h"
#include "core_int_thread.h"
#include "sched_int_tick.h"

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

	init_cpu_core(0);
	core_int_init();
	sched_int_tick(get_main_thread_handle());

	return 0;
}
