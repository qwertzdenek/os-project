#pragma once

class SMP
{
public:
	static const int numberOfcores = 4;
	SMP();
	~SMP();
	void start();
private:
	void initTCB();
	void initCores();
};

