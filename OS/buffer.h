#pragma once

/* 
 * File:   buffer.h
 * Author: tcigler
 * 
 * Created on Nov 10, 2015
 */

#ifndef BUFFER_H
#define	BUFFER_H

#define	BUFFER_SIZE 512

class Buffer {
public:
    Buffer();
    virtual ~Buffer();
    
    void add(int number) {
        if(producerIndex == BUFFER_SIZE) {
            producerIndex = 0;
        }
        
        buffer[producerIndex++] = number;
    }
    
    int remove() {
        if(consumerIndex == BUFFER_SIZE) {
            consumerIndex = 0;
        }
        
        return buffer[consumerIndex++];
    }
        
private:
    int buffer[BUFFER_SIZE];
    int producerIndex;
    int consumerIndex;
};

#endif	/* BUFFER_H */

