#include <map>
#include <cstring>
#include <iostream>
#include "tun.hpp"
#include "fragmentBuffer.hpp"

// Constructor setting nbrExpected default value 
PacketItem::PacketItem() : nbrExpected(-1) {};

// The buffers
static std::map<int, PacketItem> fragmentBuffers[NBR_BUFFERS];
static std::vector<int> doneBuffers[NBR_BUFFERS];

void addFragment(DataFrame* item, int bufferNbr) 
{
    std::cout << "Fragemt with id: " << item->id << std::endl;

    // If key doesn't exits, create a packet item
    if(fragmentBuffers[bufferNbr].find(item->id) == fragmentBuffers[bufferNbr].end()) {
        fragmentBuffers[bufferNbr].insert({ item->id, PacketItem() });
    }

    // Add fragment to list
    fragmentBuffers[bufferNbr][item->id].fragments.push_back(item);
    
    // Check if end
    if(item->end)
        fragmentBuffers[bufferNbr][item->id].nbrExpected = item->packet_num + 1;

    if(fragmentBuffers[bufferNbr][item->id].nbrExpected == fragmentBuffers[bufferNbr][item->id].fragments.size()) {
        std::cout << "Packet pushed to done, id: " << item->id << std::endl;
        doneBuffers[bufferNbr].push_back(item->id);
    }
}

char* createPacket(int id, int* size, int bufferNbr) 
{
    // Returns null if not all fragments are recivied
    if(fragmentBuffers[bufferNbr][id].nbrExpected != fragmentBuffers[bufferNbr][id].fragments.size()) {
        return nullptr;
    }

    int totalSize = 30 * fragmentBuffers[bufferNbr][id].fragments.size();
    char* packet = new char[totalSize];

    while (!fragmentBuffers[bufferNbr][id].fragments.empty())
    {
        // TODO: Add memory deallocation
        DataFrame* item = fragmentBuffers[bufferNbr][id].fragments.back();
        fragmentBuffers[bufferNbr][id].fragments.pop_back();

        std::memcpy(packet + 30 * item->packet_num, item->data, 30);
    }
    *size = packet_len(packet);
    
    // erases packet item from map once a packet is built
    fragmentBuffers[bufferNbr].erase(fragmentBuffers[bufferNbr].find(id));
    return packet;
}

int getNextId(int bufferNbr)
{
    if(doneBuffers[bufferNbr].empty())
    {
        return -1;
    } 
    int returnValue = doneBuffers[bufferNbr].back();
    doneBuffers[bufferNbr].pop_back();
    return returnValue;
}