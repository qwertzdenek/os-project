#ifndef TIMER_HPP
#define TIMER_HPP

class timer
{
private:
  std::mutex access;
public:
  void schedule(smp_context);
  void run();
}

#endif
