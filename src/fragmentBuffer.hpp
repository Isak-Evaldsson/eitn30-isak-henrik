#ifndef FRAGMENT_BUFFER_H
#define FRAGMENT_BUFFER_H
#include <vector>
#include <cstdint>
#include "frames.hpp"

// Store fragments for a given packet id
class PacketItem {
    public:
        PacketItem();
    
    public:
        int nbrExpected;
        std::vector<DataFrame*> fragments;
};

void addFragment(DataFrame* item);
char* createPacket(int id, int* size);
int getNextId();
#endif