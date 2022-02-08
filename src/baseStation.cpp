#include <cstdio>
#include <deque>
#include <iostream>
#include <assert.h>
#include <RF24/RF24.h>
#include "tun.hpp"


#define PAYLOAD_SIZE 32

RF24 rxRadio(17, 0);

uint8_t rxBuffer[PAYLOAD_SIZE];

int main()
{

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
                print_header(rxBuffer);
            }
        }
}