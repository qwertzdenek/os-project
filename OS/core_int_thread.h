#pragma once

// SetEvent on them to make interrupt
extern HANDLE scheduler_interrupt_handle[];
extern HANDLE start_interrupt_handle[];
extern HANDLE stop_interrupt_handle[];

void core_int_init(int core_number);
