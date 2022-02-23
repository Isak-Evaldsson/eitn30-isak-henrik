#ifndef TRANSBUFF_H
#define TRANSBUFF_H
#include "frames.hpp"
#include <cstdio>

void pushBufferItem(DataFrame* frame);
DataFrame* popBufferItem();

#endif