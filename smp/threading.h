/*
 *  KIV/OS
 *  Ukazkovy priklad.
 *  txkoutny@kiv.zcu.cz
*/

#pragma once

#include <Windows.h>
#include <stdbool.h>

/*
  Samovysvetlujici nazvy, podrobnosti jsou v threading.c
*/

#define kivos_MaxThreads  25

void kivos_InitThreading();
bool kivos_AddThreadingProc(LPTHREAD_START_ROUTINE proc, LPVOID lpThreadParameter);
void* kivos_GetCurrentThreadParam();
void* kivos_GetCurrentThreadId();
void kivos_RunThreading();
void kivos_EndThreading();