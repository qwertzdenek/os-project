#ifndef SMP_HPP
#define SMP_HPP

#include <string>

typedef struct {
  unsigned int count;
  std::string name;
} smp_cpu_attrs;

typedef struct {
  long generator_state;
  int mean_value;
  int variance_value;
  // anything other we want to store...
} smp_context;

class smp
{
private:
  std::vector<std::mutex> core_locks;
  std::vector<std::thread> cores;
public:
  void cpuinfo(smp_cpu_attrs &attrs);
  void schedule_task(int core, void (func *) (smp_context));
}

#endif
