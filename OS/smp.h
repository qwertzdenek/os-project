#pragma once

class SMP
{
public:
	static const int numberOfcores;
	SMP();
	~SMP();
	void start();
private:
	void initTCB();
	void initCores();
};

