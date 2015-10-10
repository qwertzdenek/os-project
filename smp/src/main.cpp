#include <iostream>

#include "core/smp.hpp"

int main(int argc, char **argv)
{
    smp_cpu_attrs cpuinfo;

	std::cout << "Hello World!" << std::endl;

    smp_cpuinfo(cpuinfo);
    std::cout << "CPU " << cpuinfo.name << " with " << cpuinfo.count;
    std::cout << " available threads" << std::endl;

	return 0;
}
