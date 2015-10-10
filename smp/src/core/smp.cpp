#include <thread>

#include "smp.hpp"

void smp_cpuinfo(smp_cpu_attrs &attrs)
{
    unsigned int n = std::thread::hardware_concurrency();
    std::string name = "KIV/OS SMP simulated processor";
    
    attrs.count  = n;
    attrs.name = name;
}

