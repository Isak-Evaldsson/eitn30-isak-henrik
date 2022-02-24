#include <iostream>
#include <assert.h>
#include "transmittBuffer.hpp"


void TransmittBuffer::pushDataFrame(DataFrame* item) {
    buffer.push_back(item);
}

bool TransmittBuffer::dataInQueue() {
    return buffer.size() > 0;
}

DataFrame* TransmittBuffer::popDataFrame() {
    if (buffer.size() <= 0) {
        return NULL;
    }

    DataFrame* ret = buffer.front();
    buffer.pop_front();

    return ret;
}