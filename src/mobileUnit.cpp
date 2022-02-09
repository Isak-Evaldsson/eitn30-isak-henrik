#include <iostream>
#include <assert.h>
#include <pthread.h>
#include <RF24/RF24.h>
#include "transmittBuffer.hpp"
#include "tun.hpp"

#define PAYLOAD_SIZE 32

RF24 txRadio(27, 60);

char txBuffer[PAYLOAD_SIZE];

void* checkBuffer(void* arg) {
    BufferItem* buf;

    while (true)
    {
        if((buf = popBufferItem()) != NULL) {
            std::cout << "Sending fragment with n: " << buf->packet_num << " with Id:" << buf->id << std::endl;
            assert(buf->getData() != NULL);

            memcpy(txBuffer + 2, buf->getData(), buf->getSize());
            txBuffer[1] = buf->id & 0xff;
            txBuffer[0] = buf->id >> 8;
            txBuffer[0] |= buf->packet_num << 2;
            txBuffer[0] = buf->start ? txBuffer[0] | 0b10000000 : txBuffer[0] & 0b0111111;
            
            

            bool succ = txRadio.write(txBuffer, PAYLOAD_SIZE);

            if (!succ)
            {
                std::cout << "Transamission failed" << std::endl;
            }
        }
    }   
}

int main(int argc, char const *argv[])
{
    pthread_t thread;
    pthread_t thread1;
    uint8_t address[6] = {"0Node"};
    
    //RF24 setup
    if (!txRadio.begin())
    {
        std::cout << "Transmitter hardware is not responding!!" << std::endl;
        exit(1);
    }
    txRadio.setPayloadSize(PAYLOAD_SIZE);
    txRadio.setPALevel(RF24_PA_LOW);
    txRadio.openWritingPipe(address);
    txRadio.stopListening();


    pthread_create(&thread, NULL, &checkBuffer, NULL);
    pthread_create(&thread1, NULL, &startInterface, NULL);

    pthread_join(thread, NULL);
    pthread_join(thread1, NULL);

    return 0;
}
