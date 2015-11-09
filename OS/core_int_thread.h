#pragma once

// SetEvent on them to make interrupt
extern HANDLE scheduler_interrupt_handle[];
extern HANDLE start_interrupt_handle[];
extern HANDLE stop_interrupt_handle[];

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Ititializes interupts thread for core. </summary>
///
/// <remarks>	David 000, 08.11.2015. </remarks>
///
/// <param name="core_number">	The core number. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////
void core_int_init(int core_number);
