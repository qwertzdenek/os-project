#pragma once

#include "stdafx.h"

#define ThreadTimeQuantum 52

typedef enum test {
	RUNNING, READY, BLOCKED, TERMINATED
} EThreadState;

typedef struct {
	CONTEXT OSContext;
	HANDLE StackHandle;
	void* Stack;
	int Id;
	EThreadState state;
} TThreadControlBlock;
