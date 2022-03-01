#include <iostream>
#include <assert.h>
#include "transmittBuffer.hpp"

TransmittBuffer::TransmittBuffer() {
    pthread_mutex_init(&mutex, NULL);
}

void TransmittBuffer::pushDataFrame(DataFrame* item) {
    pthread_mutex_lock(&mutex);
    buffer.push_back(item);
    pthread_mutex_unlock(&mutex);
}

bool TransmittBuffer::dataInQueue() {
    pthread_mutex_lock(&mutex);
    int size = buffer.size();
    pthread_mutex_unlock(&mutex);

    return size > 0;
}

DataFrame* TransmittBuffer::popDataFrame() {
    pthread_mutex_lock(&mutex);
    if (buffer.size() <= 0) {
        pthread_mutex_unlock(&mutex);
        return NULL;
    }

    DataFrame* ret = buffer.front();
    buffer.pop_front();
    pthread_mutex_unlock(&mutex);

    return ret;
}