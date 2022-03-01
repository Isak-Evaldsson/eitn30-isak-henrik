#ifndef TRANSBUFF_H
#define TRANSBUFF_H
#include "frames.hpp"
#include <cstdio>
#include <deque>
#include <map>
#include <pthread.h>

class TransmittBuffer {
private:
    std::deque<DataFrame*> buffer;
    pthread_mutex_t mutex;

public:
    void pushDataFrame(DataFrame* frame);
    bool dataInQueue();
    DataFrame* popDataFrame();
    TransmittBuffer();
};

#endif