#pragma once

/*
* File:   buffer.h
* Author: tcigler
*
* Created on Nov 10, 2015
*/

#define	BUFFER_SIZE 512

struct circular_buffer {
	int producerIndex = 0;
	int consumerIndex = 0;

	double buffer[BUFFER_SIZE];

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
};
