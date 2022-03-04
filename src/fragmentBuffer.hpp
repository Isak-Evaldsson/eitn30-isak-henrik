#ifndef FRAGMENT_BUFFER_H
#define FRAGMENT_BUFFER_H
#include <vector>
#include <cstdint>
#include <pthread.h>
#include "frames.hpp"

// Store fragments for a given packet id
class PacketItem {
    public:
        PacketItem();
    
    public:
        int nbrExpected;
        std::vector<DataFrame*> fragments;
};

void addFragment(DataFrame* item, int bufferNbr);
char* createPacket(int& size, int bufferNbr);
void setNbrOfBuffers(int nBuffers);
int getNbrOfBuffers();
#endif