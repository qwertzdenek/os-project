#include "task_init.h"
#include "sched_calls.h"
#include "task_sched.h"

unsigned WINAPI core_thread(void* param) {
	_endthreadex(0);
	return 0;
}

void exec_task(std::vector<std::string> arg)
{
	// TODO
}

int semaphore_P(semaphore_t &s)
{
	// TODO
	return 0;
}

void semaphore_V(semaphore_t &s)
{
	// TODO
}

void exit_task(int result_code)
{
	// TODO
}

unsigned WINAPI instance_start(void * param)
{
	/*
	auto *param_cast = static_cast<start_param *>(param);
	Task *task = param_cast->task_class;
	return task->start(param_cast->data);
	*/
	return 0;
}

Scheduler::Scheduler() {
	memset(&default_context, 0, sizeof(default_context));
	HANDLE CPUCore = (HANDLE)_beginthreadex(NULL, 0, &core_thread, 0, CREATE_SUSPENDED, NULL);

	default_context.ContextFlags = CONTEXT_ALL;
	GetThreadContext(CPUCore, &default_context);
}

Scheduler::~Scheduler()
{
	// TODO: clean all
}

bool Scheduler::create_task(std::string name)
{
	TTaskControlBlock tcb;
	Task task;

	// TODO: create Task instance using name

	tcb.context = default_context;

	#ifdef allocheap
		tcb.stack_handle = HeapCreate(0, ThreadStackSize, 0);
		tcb.stack = HeapAlloc(tcb.stack_handle, HEAP_NO_SERIALIZE, ThreadStackSize);
	#else
		tcb.stack = _aligned_malloc(ThreadStackSize, 64);
	#endif

	// TODO: po�e�it vol�n� OOP funkce instance_start
	#ifdef _M_X64
		//32B je pro __fastcall 
		tcb.context.Rsp = (DWORD64)tcb.stack + ThreadStackSize - 32 - sizeof(void*);
		tcb.context.Rip = (DWORD64)instance_start;

		//64-bitova aplikace si preda prvni celociselny paramter v rcx - __fastcall
		tcb.context.Rcx = (DWORD64) &task;
	#else
		//sizeof(void*) je pro nas parametr
		tcb.context.Esp = (DWORD32)tcb.stack + ThreadStackSize - 2 * sizeof(void*);
		tcb.context.Eip = (DWORD32)instance_start;

		//32-bitova aplikace si predava parametry na zasobniku - volaci konvence __stdcall
		void **tmp = (void**) &tcb.context.Esp;
		tmp++;
		*tmp = &task;
	#endif

	return false;
}

int Scheduler::main(std::vector<std::string> arg)
{
	// TODO

	return 0;
}
