#include <thread>

#include "smp.hpp"
#include "timer.hpp"

void smp::cpuinfo(smp_cpu_attrs &attrs)
{
  unsigned int n = std::thread::hardware_concurrency();
  std::string name = "KIV/OS SMP simulated processor";

  attrs.count  = n;
  attrs.name = name;
}

void smp::schedule_task(int core, void (func *) (smp_context))
{
  std::lock_guard<std::mutex> lk(core_lock);

}

void smp::smp()
{
  smp_cpu_attrs attrs;
  thread_count = cpuinfo(attrs).count;


  for (int i = 0; i < thread_count; i++) {
    std::shared_ptr<timer> t(new timer);
    std::thread t(&timer::run, t)
  }
}
