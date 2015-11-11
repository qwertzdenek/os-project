#include "stdafx.h"
#include "task_sched.h"
#include "sched_calls.h"

#include "common.h"

#include "task_init.h"
#include "task_producent.h"
#include "task_consument.h"

unsigned WINAPI core_thread(void* param) {
	_endthreadex(0);
	return 0;
}

// argv will be NULL terminated list
void exec_task(std::string name, std::string *argv[])
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

Scheduler::TaskDescriptor::TaskDescriptor(Task &task, int timeQuantum) : task(task), timeQuantum(timeQuantum)
{
}

void Scheduler::run()
{
	processNewTasks();
	processQueue();
}

void Scheduler::processNewTasks()
{
	while (!this->newTaskQueue.empty())
	{
		Task task = this->newTaskQueue.front();
		this->newTaskQueue.pop();
		Scheduler::TaskDescriptor *descriptor = new Scheduler::TaskDescriptor(task, Scheduler::TIME_QUANTUM);
		this->taskQueue.push(*descriptor);
	}
}

void Scheduler::processQueue()
{
	for (int i = 1; i <= SMP::NUMBER_OF_CORES; i++) {
		Scheduler::TaskDescriptor currentTask = *this->runningTasks[i];
		currentTask.timeQuantum -= Scheduler::TIME_QUANTUM_DECREASE;
		if (currentTask.timeQuantum <= 0) {
			Scheduler::TaskDescriptor newTask = this->taskQueue.front();
			this->taskQueue.pop();
			currentTask.timeQuantum = Scheduler::TIME_QUANTUM;
			this->taskQueue.push(currentTask);
			this->runTask(newTask);
			this->runningTasks[i] = &newTask;
		}
	}
	
	
}

void Scheduler::runTask(TaskDescriptor &taskDescriptor)
{
	// TODO
}

bool Scheduler::create_task(std::string name, std::string *argv[])
{
	task_control_block tcb;
	Task task;

	// TODO: pass arguments... don't know how
	if (name.compare("init")) {
		task = Init();
	} else if (name.compare("producent")) {
		task = Producent();
	} else if (name.compare("consument")) {
		task = Consument();
	}

	// TODO: create Task instance using name

	tcb.context = default_context;

	#ifdef allocheap
		tcb.stack_handle = HeapCreate(0, ThreadStackSize, 0);
		tcb.stack = HeapAlloc(tcb.stack_handle, HEAP_NO_SERIALIZE, ThreadStackSize);
	#else
		tcb.stack = _aligned_malloc(ThreadStackSize, 64);
	#endif

	// TODO: poøešit volání OOP funkce instance_start
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
