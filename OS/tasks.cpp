#include "stdafx.h"
#include "sched_calls.h"

#include <random>

#include "tasks.h"

const std::string task_state_names[] = { "RUNNABLE", "BLOCKED", "RUNNING", "TERMINATED" };

const std::string task_type_names[] = { "RUNNER", "CONSUMENT", "PRODUCENT", "IDLE" };

void *task_entry_points[4] = { task_main_runner, task_main_consument,
							   task_main_producent, task_main_idle };

DWORD task_main_idle(void *in)
{
	while (1)
		;
	return 0;
}

DWORD task_main_runner(void *in)
{
	task_run_parameters *params = (task_run_parameters *)in;
	// init common memory and call exec_task
	std::shared_ptr<task_common_pointers> ptr(new task_common_pointers);

	ptr->empty._value = ATOMIC_VAR_INIT(BUFFER_SIZE);
	ptr->mutex._value = ATOMIC_VAR_INIT(1);
	ptr->full._value = ATOMIC_VAR_INIT(0);

	ptr->mean = params->mean;
	ptr->deviation = params->deviation;

	ptr->can_run = true;

	int prod_id = exec_task(PRODUCENT, ptr);
	int cons_id = exec_task(CONSUMENT, ptr);

	return 0;
}

DWORD task_main_producent(void *in)
{
	task_common_pointers *task = (task_common_pointers *)in;

	std::random_device rd;
	std::default_random_engine generator;
	std::normal_distribution<double> distribution(task->mean, task->deviation);
	generator.seed(rd());

	int i = 0;
	while (task->can_run) {
		double generatedNumber = distribution(generator);

		semaphore_P(task->empty, 1);

		semaphore_P(task->mutex, 1);

		task->buffer.add(generatedNumber);

		semaphore_V(task->mutex, 1);

		semaphore_V(task->full, 1);
	}

	return 0;
}

DWORD task_main_consument(void *in)
{
	task_common_pointers *task = (task_common_pointers *)in;

	double original_mean = task->mean;
	double original_deviation = task->deviation;

	double mean = 0;
	double meanBefore = 0;
	double variance = 0;
	double deviation = 0;
	double varianceBefore = 0;

	double mean_diff, deviation_diff;

	int countConsumed = 0;

	while (task->can_run) {
		// remove one number from buffer if any
		semaphore_P(task->full, 1);
		semaphore_P(task->mutex, 1);

		double nr = task->buffer.remove();

		semaphore_V(task->mutex, 1);
		semaphore_V(task->empty, 1);

		/* count needed values */
		meanBefore = mean;
		varianceBefore = variance;

		mean = ((countConsumed * mean) + nr) / (countConsumed + 1);
		variance = (countConsumed * (varianceBefore + pow(meanBefore, 2.0)) + pow(nr, 2.0)) / (countConsumed + 1) - pow(mean, 2.0);
		deviation = sqrt(variance);

		countConsumed++;

		mean_diff = fabs(original_mean - mean);
		deviation_diff = fabs(original_deviation - deviation);

		if (mean_diff < PRECISION && deviation_diff < PRECISION) {
			// stop producer
			task->can_run = false;
		}
	}

	return 0;
}
