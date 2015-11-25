#pragma once

#include "cpu.h"

extern HANDLE core_handles[CORE_COUNT];

void init_cpu_core(int core_number);
int actual_core();

void esp_push(DWORD *esp, DWORD value);
DWORD esp_pop(DWORD *esp);
