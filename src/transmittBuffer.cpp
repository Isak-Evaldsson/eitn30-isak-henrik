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

DataFrame* popDataFrame() {
    if (buffer.size() <= 0) {
        return NULL;
    }

    DataFrame* ret = buffer.front();
    buffer.pop_front();

    return ret;
}