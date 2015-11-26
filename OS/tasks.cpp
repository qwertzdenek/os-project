#include "stdafx.h"
#include "sched_calls.h"

#include "tasks.h"

// TODO: here we can define loop end ("politely" stop the process)
// Note: maybe we should pass and check some atomic parameter in common pointers..?

bool canRun(task_common_pointers *in) {
    return true;
}

void *task_entry_points[4] = { task_main_runner, task_main_consument,
							   task_main_producent, task_main_idle};

DWORD __stdcall task_main_idle(void *)
{
	while (1)
		;
}

DWORD __stdcall task_main_runner(void *in)
{
	// init common memory and call exec_task
	task_common_pointers *struct_ptr = new task_common_pointers;

        struct_ptr->empty._value = ATOMIC_VAR_INIT(BUFFER_SIZE);
        struct_ptr->mutex._value = ATOMIC_VAR_INIT(1);

	int prod_id = exec_task(PRODUCENT, struct_ptr);
	int cons_id = exec_task(CONSUMENT, struct_ptr);

	// wait for them to exit
	wait_task(prod_id);
	wait_task(cons_id);

	exit_task();
	return 0;
}


DWORD __stdcall task_main_producent(void *in)
{

    int i = 0;
//    TODO: pass as argument
    double lambda = 1;
    while (canRun()) {
//        TODO: to statistics
//        std::printf("+++ Empty: %d\n", (int) in->empty._value);
//        std::printf("+++ Full: %d\n", (int) in->full._value);

        double randomNumber = (double) rand() / RAND_MAX;

        double hyperexponential = log(1 - randomNumber) / -lambda;

        semaphore_P(in->empty, 1);

        semaphore_P(in->mutex, 1);

        in->buffer.add(hyperexponential);
        
//        TODO: to statistics
//        std::printf("+++ Producer added: %.2f\n", hyperexponential);

        semaphore_V(in->mutex, 1);

        semaphore_V(in->full, 1);

        //        TODO: Do we need (or want) sleep for statistics and simulation?
        //        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }


	exit_task();
	return 0;
}


DWORD __stdcall task_main_consument(void *in)
{
    double mean = 0;
    double meanBefore = 0;
    int countConsumed = 0;

    while (canRun(in)) {

        // remove one place from buffer if any
        semaphore_P(in->full, 1);

        semaphore_P(in->mutex, 1);

        double nr = in->buffer.remove();

        meanBefore = mean;
        mean = meanBefore + (nr - meanBefore) / ++countConsumed;

//        TODO: to statistics
//        std::printf("--- Consumer removed: %.2f, current mean %.5f\n", nr, mean);

        semaphore_V(in->mutex, 1);

        // notify free space
        semaphore_V(in->empty, 1);

        //        TODO: Do we need (or want) sleep for statistics and simulation?
        //        std::this_thread::sleep_for(std::chrono::milliseconds(80));

    }
    
	exit_task();

}
