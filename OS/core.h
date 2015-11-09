#pragma once



/// <summary>	Array of handles for cpu cores. </summary>
extern HANDLE core_handle[];

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Initialises the CPU core. </summary>
///
/// <remarks>	David 000, 08.11.2015. </remarks>
///
/// <param name="core_number">	The core number. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////
void init_cpu_core(int core_number);
void deinit_cpu_core();
