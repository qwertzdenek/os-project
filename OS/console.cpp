// console.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <string.h>

#include "core.h"
#include "core_int_thread.h"
#include "sched_int_tick.h"
#include "task_sched.h"
#include "smp.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Prints the help. </summary>
///
/// <remarks>	David 000, 08.11.2015. </remarks>
///
/// <returns>	A std::string. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Main entry-point for this application. </summary>
///
/// <remarks>	David 000, 08.11.2015. </remarks>
///
/// <param name="argc">	Number of command-line arguments. </param>
/// <param name="argv">	Array of command-line argument strings. </param>
/// <param name="envp">	Array of command-line argument strings. </param>
///
/// <returns>	Exit-code for the process - 0 for success, else an error code. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
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
	core_int_init(0);
	HANDLE quit_flag = sched_tick_int_init();

	return 0;
}

void init()
{
	SMP *smp = new SMP();
	smp->start();
}
