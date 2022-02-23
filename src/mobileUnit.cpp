#include <iostream>
#include <assert.h>
#include <pthread.h>
#include <RF24/RF24.h>
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

bool allowedtoSend = false;
int timeToSend = 0;
unsigned int myIP = 3232235522; 


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
    while (true)
    {
        if (inCtrlQueue.size() > 0)
        {
            ControlFrame *frame = inCtrlQueue.front();
            inCtrlQueue.pop_front();

            if (frame->type == ack)
            {
                std::cout << "Recivied ack" << std::endl;
                allowedtoSend = true;

                timeToSend = time(nullptr) + frame->time;
            }
            else if (frame->type == propose)
            {
                std::cout << "Got propose" << std::endl;
                outCtrlQueue.push_back(new ControlFrame(replyYes, myIP, 0));
            }
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
            bool ok = txRadio.write(data, PAYLOAD_SIZE);

            if(!ok) {
                std::cout << "transmission failed" << std::endl;
            }
        }
    }
}

int main(int argc, char const *argv[])
{
    pthread_t ctrlThread;
    pthread_t rxThread;
    pthread_t txThread;
    uint8_t bsAddress[6] = "0Node";
    uint8_t myAddress[6] = "1Node";

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
    rxRadio.setChannel(112);
    rxRadio.openReadingPipe(1, myAddress);
    rxRadio.startListening();

    // setup transmitter
    txRadio.setPayloadSize(PAYLOAD_SIZE);
    txRadio.setPALevel(RF24_PA_LOW);
    txRadio.setChannel(111);
    txRadio.openWritingPipe(bsAddress);
    txRadio.stopListening();

    //setup("192.168.0.2/24");

    pthread_create(&ctrlThread, NULL, &controlThread, NULL);
    pthread_create(&rxThread, NULL, &reciveFragments, NULL);
    pthread_create(&txThread, NULL, &transmitterThread, NULL);

    pthread_join(ctrlThread, NULL);
    pthread_join(rxThread, NULL);
    pthread_join(txThread, NULL);

    return 0;
}
