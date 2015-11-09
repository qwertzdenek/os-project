#include "stdafx.h"

#include "smp.h"
#include "core.h"

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
	for (int i = 1; i < NUMBER_OF_CORES; i++) {
		init_cpu_core(i);
	}
}

