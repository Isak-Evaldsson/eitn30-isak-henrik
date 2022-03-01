#ifndef FRAGMENT_BUFFER_H
#define FRAGMENT_BUFFER_H
#include <vector>
#include <cstdint>
#include <pthread.h>
#include "frames.hpp"

// Implementation currently specifies a fixed number of fragment buffers
#define NBR_BUFFERS (2)

// Store fragments for a given packet id
class PacketItem {
    public:
        PacketItem();
    
    public:
        int nbrExpected;
        std::vector<DataFrame*> fragments;
};

void addFragment(DataFrame* item, int bufferNbr);
char* createPacket(int id, int* size, int bufferNbr);
int getNextId(int bufferNbr);

#endif