#pragma once

#include <string>

#include "sched.h"

// it will add this task to the exec list
// this memory shoud be copied to the scheduler scope
// argv: NULL terminated list
extern void exec_task(std::string name, std::string *argv[]);

// accuire semaphore and return his value
extern int semaphore_P(semaphore_t &s);

// release semaphore and return his value
extern void semaphore_V(semaphore_t &s);

// exits the calling process
extern void exit_task(int result_code);
