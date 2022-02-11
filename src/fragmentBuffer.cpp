#include <map>
#include <cstring>
#include "fragmentBuffer.hpp"

// Constructor setting nbrExpected default value 
PacketItem::PacketItem() : nbrExpected(-1) {};

// Map acting as a buffer for all recived fragments
std::map<int, PacketItem> fragmentBuffer;

void addFragment(BufferItem* item) {
    // If key doesn't exits, create a packet item
    if(fragmentBuffer.find(item->id) == fragmentBuffer.end()) {
        fragmentBuffer[item->id] = PacketItem();
    }

    // Add fragment to list
    fragmentBuffer[item->id].fragments.push_back(item);
    
    // Check if end
    if(item->end)
        fragmentBuffer[item->id].nbrExpected = item->packet_num + 1;
}

uint8_t* createPacket(int id) {
    // Returns null if not all fragments are recivied
    if(fragmentBuffer[id].nbrExpected != fragmentBuffer[id].fragments.size()) {
        return nullptr;
    }

    uint8_t* packet = new uint8_t[30 * fragmentBuffer[id].fragments.size()];
    
    while (!fragmentBuffer[id].fragments.empty())
    {
        // TODO: Add memory deallocation
        BufferItem* item = fragmentBuffer[id].fragments.back();
        fragmentBuffer[id].fragments.pop_back();

        std::memcpy(packet + item->packet_num, item->data + 2, 30);
    }
    
    // erases packet item from map once a packet is built
    fragmentBuffer.erase(fragmentBuffer.find(id));
    return packet;
}