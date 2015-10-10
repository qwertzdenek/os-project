#ifndef SMP_HPP
#define SMP_HPP

#include <string>

typedef struct {
    unsigned int count;
    std::string name;
} smp_cpu_attrs;

void smp_cpuinfo(smp_cpu_attrs &attrs);

#endif

