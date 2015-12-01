#include "stdafx.h"
#include "sched_calls.h"

#include "tasks.h"

const std::string task_state_names[] = { "RUNNABLE", "BLOCKED", "RUNNING", "TERMINATED" };

const std::string task_type_names[] = { "RUNNER", "CONSUMENT", "PRODUCENT", "IDLE" };

// TODO: here we can define loop end ("politely" stop the process)
// Note: maybe we should pass and check some atomic parameter in common pointers..?
bool canRun(task_common_pointers *in) {
	return true;
}

void *task_entry_points[4] = { task_main_runner, task_main_consument,
							   task_main_producent, task_main_idle };

DWORD task_main_idle(void *)
{
	while (1)
		;
	return 0;
}

DWORD task_main_runner(void *in)
{
	// init common memory and call exec_task
	std::shared_ptr<task_common_pointers> ptr(new task_common_pointers);

	ptr->empty._value = ATOMIC_VAR_INIT(BUFFER_SIZE);
	ptr->mutex._value = ATOMIC_VAR_INIT(1);
	ptr->full._value = ATOMIC_VAR_INIT(0);

	int prod_id = exec_task(PRODUCENT, ptr);
	int cons_id = exec_task(CONSUMENT, ptr);

	// wait for them to exit
	wait_task(prod_id);
	wait_task(cons_id);

	return 0;
}

DWORD task_main_producent(void *in)
{
	task_common_pointers *task = (task_common_pointers *)in;
	int i = 0;

	double lambda = 1;
	while (canRun(NULL)) {
		double randomNumber = (double)rand() / RAND_MAX;

		double hyperexponential = log(1 - randomNumber) / -lambda;

		semaphore_P(task->empty, 1);

		semaphore_P(task->mutex, 1);

		task->buffer.add(hyperexponential);

		//        TODO: to statistics
		//        std::printf("+++ Producer added: %.2f\n", hyperexponential);

		semaphore_V(task->mutex, 1);

		semaphore_V(task->full, 1);

		//        TODO: Do we need (or want) sleep for statistics and simulation?
		//        std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	return 0;
}

DWORD task_main_consument(void *in)
{
	task_common_pointers *task = (task_common_pointers *)in;

	double mean = 0;
	double meanBefore = 0;
	int countConsumed = 0;

	while (canRun(task)) {
		// remove one place from buffer if any
		semaphore_P(task->full, 1);

		semaphore_P(task->mutex, 1);

		double nr = task->buffer.remove();

		meanBefore = mean;
		mean = meanBefore + (nr - meanBefore) / ++countConsumed;

		//        TODO: to statistics
		//        std::printf("--- Consumer removed: %.2f, current mean %.5f\n", nr, mean);

		semaphore_V(task->mutex, 1);

		// notify free space
		semaphore_V(task->empty, 1);

		//        TODO: Do we need (or want) sleep for statistics and simulation?
		//        std::this_thread::sleep_for(std::chrono::milliseconds(80));

	}

	return 0;
}
