#pragma once

#define CORE_COUNT 4

extern HANDLE core_handle[CORE_COUNT];

void init_cpu_core(int core_number);
void deinit_cpu_core();
