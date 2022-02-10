#include <cstdio>
#include <deque>
#include <iostream>
#include <assert.h>
#include <RF24/RF24.h>
#include <functional>
#include <queue>
#include <vector>
#include "tun.hpp"
#include "transmittBuffer.hpp"


#define PAYLOAD_SIZE 32

RF24 rxRadio(17, 0);

uint8_t rxBuffer[PAYLOAD_SIZE];

int main()
{
    auto cmp = [](BufferItem *left, BufferItem *right)
    {
        return left->packet_num < right->packet_num;
    }; 

    std::priority_queue<BufferItem*, std::vector<BufferItem*>, decltype(cmp)> q(cmp);
    int totExpNbrP = -1; //Total expected number of packets


    uint8_t address[6] = {"0Node"};

    //RF24 setup
    if (!rxRadio.begin())
    {
        std::cout << "Reciver hardware is not responding!!" << std::endl;
        exit(1);
    }

    // setup reciver
    rxRadio.setPayloadSize(PAYLOAD_SIZE);
    rxRadio.setPALevel(RF24_PA_LOW);
    rxRadio.openReadingPipe(1, address);
    rxRadio.startListening();

    std::cout << "Starting to listen for packets!" << std::endl;
    while (true)
        {
            if (rxRadio.available())
            {
                rxRadio.read(rxBuffer, PAYLOAD_SIZE);
                int pNbr = rxBuffer[0] & 0b01111100;
                pNbr >>= 2;
                int id =   rxBuffer[0] & 0b00000011;
                id <<= 8;
                id += rxBuffer[1];

                bool end = rxBuffer[0] & 0x80;

                std::cout << "Reciving fragment with n: " << pNbr << " with Id: " << id << " end: " << end << std::endl;
                char *data = (char*) malloc(sizeof(rxBuffer));
                memcpy(data, rxBuffer, sizeof(rxBuffer));

                BufferItem *tmp = new BufferItem(data, sizeof(rxBuffer), id, pNbr, end);
                q.push(tmp);

                if(end)
                    totExpNbrP = pNbr + 1;

                if(q.size() == totExpNbrP){
                    std::cout << "All fragments recivied" << std::endl;    

                    // Lets build a packet
                    uint8_t* packet = (uint8_t*) calloc(30, q.size());
                    while (!q.empty())
                    {
                        // TODO: Add memory deallocation
                        BufferItem* item = q.top();
                        q.pop();

                        memcpy(packet + item->packet_num, item->data + 2, 30);
                    }
                    print_header(packet);
                }

                //print_header(rxBuffer);
            }
        }
}