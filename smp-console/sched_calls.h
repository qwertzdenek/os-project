#pragma once

#include <string>

#include "sched.h"

// it will create NEW TTaskControlBlock
extern void exec_task(std::vector<std::string> arg);

// accuire semaphore and return his value
extern int semaphore_P(semaphore_t &s);

// release semaphore and return his value
extern void semaphore_V(semaphore_t &s);

// exits the calling process
extern void exit_task(int result_code);
