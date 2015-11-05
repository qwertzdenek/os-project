// smp-console.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

#include "cpu.h"
#include "task_sched.h"

using namespace SMP;

int main()
{
	CPU cpu;
	Scheduler s;

	// cpu.set_scheduler_entry(s.start);
	return 0;
}
