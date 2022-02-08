#include <cstdio>
#include <deque>
#include <iostream>
#include <assert.h>
#include "transmittBuffer.hpp"

// The Buffer
std::deque<BufferItem*> buffer;

/* 
    BufferItem class implementation
*/
BufferItem::BufferItem(char* d, size_t s) {
    data = d;
    size = s;
}

size_t BufferItem::getSize() {
    return size;
}

char* BufferItem::getData() {
    return data;
}

/* 
    Buffer methods
*/
void pushBufferItem(char* data, size_t size) {
    BufferItem *item = new BufferItem(data, size);
    buffer.push_back(item);
}

BufferItem* popBufferItem() {
    if (buffer.size() <= 0) {
        return NULL;
    }

    BufferItem* ret = buffer.front();
    buffer.pop_front();

    return ret;
}