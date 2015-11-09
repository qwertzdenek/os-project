
#include "smp.h"
#include "core.h"

const int SMP::numberOfcores = 4;

SMP::SMP()
{
}

SMP::~SMP()
{
}

void SMP::start()
{
	this->initTCB();
	this->initCores();
}

void SMP::initTCB() 
{
	// TODO
}

void SMP::initCores()
{
	for (int i = 1; i < numberOfcores; i++) {
		init_cpu_core(i);
	}
}

