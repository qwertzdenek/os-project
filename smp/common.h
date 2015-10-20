#pragma once

#include "stdafx.h"

#define ThreadTimeQuantum 52

typedef struct {
	CONTEXT OSContext;
	HANDLE StackHandle;
	void* Stack;
	int Id;
} TThreadControlBlock;
