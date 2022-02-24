#ifndef TRANSBUFF_H
#define TRANSBUFF_H
#include "frames.hpp"
#include <cstdio>

void pushDataFrame(DataFrame* frame);
void pushDataFrame(char* data, size_t size);
bool dataInQueue();
DataFrame* popDataFrame();

#endif