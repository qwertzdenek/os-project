#pragma once

#define ThreadTimeQuantum 52

typedef enum {
	NEW, RUNNABLE, BLOCKED, RUNNING, TERMINATED
} ETaskState;

typedef struct {
	CONTEXT OSContext;
	HANDLE StackHandle;
	void* Stack;
	int id;
	time_t started;
	ETaskState state;
} TTaskControlBlock;
