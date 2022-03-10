#include <iostream>
#include <assert.h>
#include <pthread.h>
#include <RF24/RF24.h>
#include "transmittBuffer.hpp"
#include "tun.hpp"
#include "fragmentBuffer.hpp"

#define PAYLOAD_SIZE 32

RF24 rxRadio(17, 0);
RF24 txRadio(27, 60);

uint8_t rxBuffer[PAYLOAD_SIZE];
uint8_t txBuffer[PAYLOAD_SIZE];

void *reciveFragments(void *arg)
{
    std::cout << "Starting to listen for packets!" << std::endl;
    while (true)
    {
        if (rxRadio.available())
        {
            rxRadio.read(rxBuffer, PAYLOAD_SIZE);
            int pNbr = rxBuffer[0] & 0b01111100;
            pNbr >>= 2;
            int id = rxBuffer[0] & 0b00000011;
            id <<= 8;
            id += rxBuffer[1];

            bool end = rxBuffer[0] & 0x80;

            std::cout << "Reciving fragment with n: " << pNbr << " with Id: " << id << " end: " << end << std::endl;
            char *data = new char[PAYLOAD_SIZE];
            memcpy(data, rxBuffer, PAYLOAD_SIZE);

            BufferItem *tmp = new BufferItem(data, PAYLOAD_SIZE, id, pNbr, end);
            addFragment(tmp);
        }
    }
}

void *checkBuffer(void *arg)
{
    BufferItem *buf;

    while (true)
    {
        if ((buf = popBufferItem()) != NULL)
        {
            std::cout << "Sending fragment with n: " << buf->packet_num << " with Id:" << buf->id << std::endl;
            assert(buf->getData() != NULL);
            hex_dump(buf->data, buf->size);

            memcpy(txBuffer + 2, buf->getData(), buf->getSize());
            txBuffer[1] = buf->id & 0xff;
            txBuffer[0] = buf->id >> 8;
            txBuffer[0] |= buf->packet_num << 2;
            txBuffer[0] = buf->end ? txBuffer[0] | 0b10000000 : txBuffer[0] & 0b0111111;

            bool succ = txRadio.write(txBuffer, buf->getSize() + 2);

            if (!succ)
            {
                std::cout << "Transamission failed" << std::endl;
            }
        }
    }
}

int main(int argc, char const *argv[])
{
    pthread_t writeThread;
    pthread_t readThread;
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
    rxRadio.setChannel(112);
    rxRadio.setDataRate(RF24_2MBPS);
    rxRadio.openReadingPipe(1, address[1]);
    rxRadio.startListening();

    // setup transmitter
    txRadio.setPayloadSize(PAYLOAD_SIZE);
    txRadio.setPALevel(RF24_PA_LOW);
    txRadio.setChannel(111);
    txRadio.setDataRate(RF24_2MBPS);
    txRadio.openWritingPipe(address[0]);
    txRadio.stopListening();

    setup("192.168.0.2/24");

    pthread_create(&writeThread, NULL, &writeInterface, NULL);
    pthread_create(&writeThread, NULL, &readInterface, NULL);
    pthread_create(&rxThread, NULL, &reciveFragments, NULL);
    pthread_create(&txThread, NULL, &checkBuffer, NULL);

    pthread_join(readThread, NULL);
    pthread_join(writeThread, NULL);
    pthread_join(rxThread, NULL);
    pthread_join(txThread, NULL);

    return 0;
}
