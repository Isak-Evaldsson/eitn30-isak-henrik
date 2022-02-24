#ifndef TRANSBUFF_H
#define TRANSBUFF_H
#include "frames.hpp"
#include <cstdio>
#include <deque>
#include <map>

class TransmittBuffer {
private:
    std::deque<DataFrame*> buffer;

public:
    void pushDataFrame(DataFrame* frame);
    bool dataInQueue();
    DataFrame* popDataFrame();
};

#endif