#include <map>
#include <cstring>
#include <iostream>
#include "tun.hpp"
#include "fragmentBuffer.hpp"

// Constructor setting nbrExpected default value 
PacketItem::PacketItem() : nbrExpected(-1) {};

void FragmentBuffer::addFragment(DataFrame* item) 
{
    // If key doesn't exits, create a packet item
    if(fragmentBuffer.find(item->id) == fragmentBuffer.end()) {
        fragmentBuffer[item->id] = PacketItem();
    }

    // Add fragment to list
    fragmentBuffer[item->id].fragments.push_back(item);
    
    // Check if end
    if(item->end)
        fragmentBuffer[item->id].nbrExpected = item->packet_num + 1;

    if(fragmentBuffer[item->id].nbrExpected == fragmentBuffer[item->id].fragments.size()) {
        doneBuffer.push_back(item->id);
    }
}

char* FragmentBuffer::createPacket(int id, int* size) 
{
    // Returns null if not all fragments are recivied
    if(fragmentBuffer[id].nbrExpected != fragmentBuffer[id].fragments.size()) {
        return nullptr;
    }

    int totalSize = 30 * fragmentBuffer[id].fragments.size();
    char* packet = new char[totalSize];

    while (!fragmentBuffer[id].fragments.empty())
    {
        // TODO: Add memory deallocation
        DataFrame* item = fragmentBuffer[id].fragments.back();
        fragmentBuffer[id].fragments.pop_back();

        std::memcpy(packet + 30 * item->packet_num, item->data + 2, 30);
    }
    *size = packet_len(packet);
    
    // erases packet item from map once a packet is built
    fragmentBuffer.erase(fragmentBuffer.find(id));
    return packet;
}

int FragmentBuffer::getNextId()
{
    if(doneBuffer.empty())
    {
        return -1;
    } 
    int returnValue = doneBuffer.back();
    doneBuffer.pop_back();

    return returnValue;
}