// console.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <string.h>

#include "core.h"
#include "core_int_thread.h"
#include "sched_int_tick.h"
#include "task_sched.h"

std::string help()
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

	init_cpu_core(0);
	core_int_init();
	HANDLE quit_flag = sched_int_tick_init();

	return 0;
}
