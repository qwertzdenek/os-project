// smp-console.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "cpu.h"

#include <iostream>

using namespace SMP;

int main()
{
	CPU cpu;

	std::cout << "Thread count: " << cpu.count() << std::endl;

	return 0;
}
