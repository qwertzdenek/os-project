#pragma once

class SMP
{
public:
	static const int NUMBER_OF_CORES = 4;
	SMP();
	~SMP();
	void start();
private:
	void initTCB();
	void initCores();
};

