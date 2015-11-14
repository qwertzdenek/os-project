/* 
 * File:   buffer.cpp
 * Author: tcigler
 * 
 * Created on Nov 10, 2015
 */

#include "buffer.h"

Buffer::Buffer() {
    producerIndex = 0;
    consumerIndex = 0;
}

Buffer::~Buffer() {
}

