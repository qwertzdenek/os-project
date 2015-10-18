#ifndef CORE_HPP
#define CORE_HPP

class core
{
private:
  std::thread work;
  std::thread timer;

public:
  void sched(smp_context);
}

#endif
