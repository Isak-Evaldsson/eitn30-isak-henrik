#include <cstdio>
#include <iostream>
#include <assert.h>
#include <RF24/RF24.h>
#include <pthread.h>
#include <deque>
#include <time.h>
#include "frames.hpp"

#define PAYLOAD_SIZE 32

RF24 rxRadio(17, 0);
RF24 txRadio(27, 60);

char rxBuffer[PAYLOAD_SIZE];
char txBuffer[PAYLOAD_SIZE];

std::deque<ControlFrame *> inCtrlQueue;
std::deque<ControlFrame *> outCtrlQueue;

void *reciveFragments(void *arg)
{
    std::cout << "Starting to listen for packets!" << std::endl;
    while (true)
    {
        if (rxRadio.available())
        {
            rxRadio.read(rxBuffer, PAYLOAD_SIZE);
            char ctrl = rxBuffer[0] & 0x80;

            if (ctrl)
            {
                std::cout << "Recived control frame" << std::endl;
                inCtrlQueue.push_back(new ControlFrame(rxBuffer));
            }
            else
            {
                // Handle data later
            }
        }
    }
}

void *controlThread(void *arg)
{
    int state = 0;

    while (true)
    {
        if (state == 0)
        {
            // sending our proposal to our only node
            outCtrlQueue.push_back(new ControlFrame(propose, 0, 0));
        }
        else if (state == 1)
        {
            // wait for timeout or reply
        }
        else
        {
            // state 3, wait for a set amout of time, reset state
        }
    }
}

void *transmitterThread(void *arg)
{
    std::cout << "Transmitting" << std::endl;
    while (true)
    {
        if (outCtrlQueue.size() > 0)
        {
            ControlFrame *frame = outCtrlQueue.front();
            outCtrlQueue.pop_front();

            char *data = frame->serialize();

            std::cout << "Sending reply yes" << std::endl;
            txRadio.write(data, PAYLOAD_SIZE);
        }
    }
}

int main()
{
    pthread_t ctrlThread;
    pthread_t rxThread;
    pthread_t txThread;
    uint8_t address[2][6] = {"0Node", "1Node"};

    //RF24 setup
    if (!rxRadio.begin())
    {
        std::cout << "Reciver hardware is not responding!!" << std::endl;
        exit(1);
    }

    if (!txRadio.begin())
    {
        std::cout << "Transmitter hardware is not responding!!" << std::endl;
        exit(1);
    }

    // setup reciver
    rxRadio.setPayloadSize(PAYLOAD_SIZE);
    rxRadio.setPALevel(RF24_PA_LOW);
    rxRadio.setChannel(111);
    rxRadio.openReadingPipe(1, address[0]);
    rxRadio.startListening();

    // setup transmitter
    txRadio.setPayloadSize(PAYLOAD_SIZE);
    txRadio.setPALevel(RF24_PA_LOW);
    txRadio.setChannel(112);
    txRadio.openWritingPipe(address[1]);
    txRadio.stopListening();

    // setup tun
    //setup("192.168.0.1/24");

    pthread_create(&ctrlThread, NULL, &controlThread, NULL);
    pthread_create(&rxThread, NULL, &reciveFragments, NULL);
    pthread_create(&txThread, NULL, &transmitterThread, NULL);

    pthread_join(ctrlThread, NULL);
    pthread_join(rxThread, NULL);
    pthread_join(txThread, NULL);
}