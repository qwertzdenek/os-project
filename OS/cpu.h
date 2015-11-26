#pragma once

#define CORE_COUNT 4
#define INTERRUPT_COUNT 16

extern bool cpu_int_table_masked[CORE_COUNT][INTERRUPT_COUNT];
extern void *cpu_int_table_routines[CORE_COUNT][INTERRUPT_COUNT];
extern HANDLE cpu_int_table_handlers[CORE_COUNT][INTERRUPT_COUNT];
extern void *cpu_int_table_messages[CORE_COUNT][INTERRUPT_COUNT];

typedef enum {
	INT_SCHEDULER, INT_RESCHEDULE, INT_CORE_TERM,
	INT_CORE_RESUME, INT_CORE_SUSPEND
} int_table_codes;
