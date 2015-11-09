#pragma once

#include "cpu.h"

extern HANDLE core_handle[];

void init_cpu_core(int core_number);
void deinit_cpu_core();
