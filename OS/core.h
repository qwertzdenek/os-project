#pragma once

#include "cpu.h"

#define THREAD_STACK_SIZE 1024

extern HANDLE core_handle[];

void init_cpu_core(int core_number);
void deinit_cpu_core();
