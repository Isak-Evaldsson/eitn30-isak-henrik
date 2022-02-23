#include <cstdio>
#include <iostream>
#include <assert.h>
#include <RF24/RF24.h>
#include <pthread.h>
#include <deque>
#include <unistd.h>
#include <vector>
#include "frames.hpp"

#define PAYLOAD_SIZE 32

// Nbr of 10 ms wait cycles before bs expects an ack
#define N_WAIT_CYCLES 10

// Nbr of milliseconds to send data
#define SEND_TIME 2000

struct DeviceEntry{
    unsigned int ip;
    uint8_t address[6];
};

std::vector<DeviceEntry> deviceTable{DeviceEntry{3232235522, "1Node"}, DeviceEntry{3232235523, "2Node"}};

RF24 rxRadio(17, 0);
RF24 txRadio(27, 60);

char rxBuffer[PAYLOAD_SIZE];
char txBuffer[PAYLOAD_SIZE];

ControlFrame* recivedCtrlFrame = nullptr;
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
                
                // Drop unhandled ctrl packges
                if(recivedCtrlFrame) {
                    delete recivedCtrlFrame;
                }
                recivedCtrlFrame = new ControlFrame(rxBuffer);
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
    int waitCycles = 0;
    int currentDevice = 0; 

    while (true)
    {
        if (state == 0)
        {
            std::cout << "State idle" << std::endl;

            //Choosing next node to send to 
            currentDevice = (currentDevice + 1) % deviceTable.size();

            // sending our proposal to our only node
            outCtrlQueue.push_back(new ControlFrame(propose, deviceTable[currentDevice].ip, 0));
            state = 1;
            waitCycles = 0;
        }
        else if (state == 1)
        {
            std::cout << "State wait for reply" << std::endl;

            // checks if we got a messgae
            if(recivedCtrlFrame && recivedCtrlFrame->ip == deviceTable[currentDevice].ip) {
                ControlFrame *frame = recivedCtrlFrame;
                recivedCtrlFrame = nullptr;

                assert(frame != NULL);

                switch(frame->type) {
                    case replyYes:
                        std::cout << "Got yes" << std::endl;
                        state = 3; // go to send state
                        continue;
                    case replyNo:
                        std::cout << "Got no" << std::endl;
                        state = 1; // back to idle state
                        continue;
                    default:
                        std::cout << "Got incorrect reply from mobile unit" << std::endl;
                        exit(1);
                        break;    
                }
            }
    
            // No reply, check for timeout
            if(++waitCycles >= N_WAIT_CYCLES) {
                std::cout << "Did not get a reply in time" << std::endl;
                state = 0;
            }

            // sleep 10 ms to ensure correct timings
            usleep(1000 * 10);
        }
        else if(state == 3)
        {
            std::cout << "time to send to: " << deviceTable[currentDevice].ip << std::endl;
            outCtrlQueue.push_back(new ControlFrame(ack, deviceTable[currentDevice].ip, SEND_TIME));
            // state 3, wait for a set amout of time, reset state
            usleep(1000 * (1.2 * SEND_TIME));
            state = 0;
        } else {
            std::cout << "Base station reached invalid state" << std::endl;
            exit(1);
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

            std::cout << "Sending request" << std::endl;

            // Find correct address in device table
            for (DeviceEntry e: deviceTable) {
                if(e.ip == frame->ip) {
                    txRadio.openWritingPipe(e.address);
                }
            }

            txRadio.write(data, PAYLOAD_SIZE);
        }
    }
}

int main()
{
    pthread_t ctrlThread;
    pthread_t rxThread;
    pthread_t txThread;
    uint8_t address[6] = {"0Node"};

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
    rxRadio.openReadingPipe(1, address);
    rxRadio.startListening();

    // setup transmitter
    txRadio.setPayloadSize(PAYLOAD_SIZE);
    txRadio.setPALevel(RF24_PA_LOW);
    txRadio.setChannel(112);
    txRadio.openWritingPipe(deviceTable[0].address);
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