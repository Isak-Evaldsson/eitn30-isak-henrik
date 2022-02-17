#ifndef FRAGMENT_BUFFER_H
#define FRAGMENT_BUFFER_H
#include <vector>
#include <cstdint>
#include "transmittBuffer.hpp"

// Store fragments for a given packet id
class PacketItem {
    public:
        PacketItem();
    
    public:
        int nbrExpected;
        std::vector<BufferItem*> fragments;
};

void addFragment(BufferItem* item);
char* createPacket(int id, int* size);
int getNextId();
#endif