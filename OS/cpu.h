#pragma once

#define CORE_COUNT 4
#define INTERRUPT_COUNT 16

extern bool cpu_int_table_masked[CORE_COUNT][INTERRUPT_COUNT];
extern void *cpu_int_table_routines[CORE_COUNT][INTERRUPT_COUNT];
extern HANDLE cpu_int_table_handlers[CORE_COUNT][INTERRUPT_COUNT];
extern void *cpu_int_table_messages[CORE_COUNT][INTERRUPT_COUNT];
