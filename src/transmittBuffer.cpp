#include <cstdio>
#include <deque>
#include <iostream>
#include <assert.h>
#include "transmittBuffer.hpp"

// The Buffer
std::deque<DataFrame*> buffer;

/* 
    Buffer methods
*/
void pushDataFrame(DataFrame* item) {
    buffer.push_back(item);
}

void pushDataFrame(char* data, size_t size) {
    DataFrame *item = new DataFrame(data, size, 0, 0, false);
    buffer.push_back(item);
}

bool dataInQueue() {
    return buffer.size() > 0;
}

DataFrame* popDataFrame() {
    if (buffer.size() <= 0) {
        return NULL;
    }

    DataFrame* ret = buffer.front();
    buffer.pop_front();

    return ret;
}