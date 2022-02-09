#ifndef TRANSBUFF_H
#define TRANSBUFF_H
#include <cstdio>

typedef class BufferItem BufferItem;

class BufferItem {
    public:
        BufferItem(char* data, size_t size, int id, int packet_num, bool start);
        size_t getSize();
        char* getData();

    public:
        char* data;
        size_t size;
        int id;
        int packet_num;
        bool start;
};

void pushBufferItem(char* data, size_t size);
void pushBufferItem(BufferItem* item);
BufferItem* popBufferItem();

#endif