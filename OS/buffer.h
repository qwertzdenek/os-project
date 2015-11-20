#pragma once

/*
* File:   buffer.h
* Author: tcigler
*
* Created on Nov 10, 2015
*/

#define	BUFFER_SIZE 512

class Buffer {
public:
	Buffer();
	virtual ~Buffer();

	void add(double number) {
		if (producerIndex == BUFFER_SIZE) {
			producerIndex = 0;
		}

		buffer[producerIndex++] = number;
	}

	double remove() {
		if (consumerIndex == BUFFER_SIZE) {
			consumerIndex = 0;
		}

		return buffer[consumerIndex++];
	}

private:
	double buffer[BUFFER_SIZE];
	int producerIndex;
	int consumerIndex;
};
