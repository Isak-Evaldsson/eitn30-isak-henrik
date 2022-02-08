#ifndef TRANSBUFF_H
#define TRANSBUFF_H
#include <cstdio>

typedef class BufferItem BufferItem;

class BufferItem {
    public:
        BufferItem(char* data, size_t size);
        size_t getSize();
        char* getData();

    private:
        char* data;
        size_t size;
};

void pushBufferItem(char* data, size_t size);
BufferItem* popBufferItem();

#endif