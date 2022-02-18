#include <cstdio>
#include <iostream>
#include <assert.h>
#include <RF24/RF24.h>
#include <pthread.h>
#include "tun.hpp"
#include "fragmentBuffer.hpp"


#define PAYLOAD_SIZE 32

RF24 rxRadio(17, 0);

uint8_t rxBuffer[PAYLOAD_SIZE];

void* reciveFragments(void *arg) {
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
            char *data = new char[PAYLOAD_SIZE];
            memcpy(data, rxBuffer, PAYLOAD_SIZE);

            hex_dump(data, PAYLOAD_SIZE);

            BufferItem *tmp = new BufferItem(data, PAYLOAD_SIZE, id, pNbr, end);
            addFragment(tmp);
        }
    }
}

int main()
{
    pthread_t tunThread;
    pthread_t rfThread;
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
    rxRadio.setChannel(111);
    rxRadio.openReadingPipe(1, address);
    rxRadio.startListening();



    pthread_create(&tunThread, NULL, &replyInterface, NULL);
    pthread_create(&rfThread, NULL, &reciveFragments, NULL);

    pthread_join(tunThread, NULL);
    pthread_join(rfThread, NULL);
}