#include <iostream>
#include <assert.h>
#include <pthread.h>
#include <RF24/RF24.h>
#include <stdarg.h>
#include <deque>
#include <time.h>
#include <chrono>

#include "frames.hpp"
#include "fragmentBuffer.hpp"
#include "transmittBuffer.hpp"
#include "tun.hpp"

#define PAYLOAD_SIZE 32

#define DEBUG 1

#if DEBUG
#define pr(...)                       \
    do                                \
    {                                 \
        fprintf(stderr, __VA_ARGS__); \
    } while (0)
#else
#define pr(...) /* no effect at all */
#endif

TransmittBuffer transmittBuffer;

RF24 rxRadio(17, 0);
RF24 txRadio(27, 60);

char rxBuffer[PAYLOAD_SIZE];
char txBuffer[PAYLOAD_SIZE];

std::deque<ControlFrame *> inCtrlQueue;
std::deque<ControlFrame *> outCtrlQueue;
pthread_mutex_t ctrlLock = PTHREAD_MUTEX_INITIALIZER;

bool allowedToSend = false;
uint64_t timeToSendEnd = 0;

unsigned int myIP = 3232235522; //Change depending of this machines IP
int id = 1;                     //Change depending of this machines IP

// utility get current time millis function
uint64_t getCurrentTimeMillis()
{
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

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
                //std::cout << "Recived control frame" << std::endl;
                pthread_mutex_lock(&ctrlLock);
                inCtrlQueue.push_back(new ControlFrame(rxBuffer));
                pthread_mutex_unlock(&ctrlLock);
            }
            else
            {
                DataFrame *frame = new DataFrame(rxBuffer);
                pr("Recvied fragment with id: %d\n", frame->id);
                addFragment(frame, 0);
            }
        }
    }
}

void *controlThread(void *arg)
{
    while (true)
    {
        pthread_mutex_lock(&ctrlLock);
        if (inCtrlQueue.size() > 0)
        {
            ControlFrame *frame = inCtrlQueue.front();
            inCtrlQueue.pop_front();

            if (frame->type == ack)
            {
                //std::cout << "Recivied ack" << std::endl;
                allowedToSend = true;
                timeToSendEnd = getCurrentTimeMillis() + frame->time;
            }
            else if (frame->type == propose)
            {
                //std::cout << "Got propose" << std::endl;
                outCtrlQueue.push_back(new ControlFrame(transmittBuffer.dataInQueue() ? replyYes : replyNo, myIP, 0));
            }
        }
        pthread_mutex_unlock(&ctrlLock);
    }
}

void *transmitterThread(void *arg)
{
    DataFrame *df;

    std::cout << "Transmitting" << std::endl;
    while (true)
    {
        pthread_mutex_lock(&ctrlLock);
        if (outCtrlQueue.size() > 0)
        {
            ControlFrame *frame = outCtrlQueue.front();
            outCtrlQueue.pop_front();
            pthread_mutex_unlock(&ctrlLock);

            char *data = frame->serialize();

            //std::cout << "Sending reply yes" << std::endl;
            bool ok = txRadio.write(data, PAYLOAD_SIZE);

            if (!ok)
            {
                std::cout << "transmission failed" << std::endl;
            }
            continue; // ensures that ctrl always will be prioritized
        }
        else
        {
            pthread_mutex_unlock(&ctrlLock);
        }

        // Checks for timeout
        if (allowedToSend && getCurrentTimeMillis() > timeToSendEnd)
        {
            allowedToSend = false;
        }

        if (allowedToSend && ((df = transmittBuffer.popDataFrame()) != NULL))
        {
            pr("Sending with id: %d\n", df->id);
            char *data = df->serialize();
            bool ok = txRadio.write(data, PAYLOAD_SIZE);

            if(!ok) {
                std::cout << "transmission failed" << std::endl;
            }
        }
    }
}

void *writeInterface(void *arg)
{
    int size;
    char *packet;
    std::cout << "writing packages to tun interface" << std::endl;

    while (true)
    {
        // Only reads packet if available
        if ((packet = createPacket(size, 0)))
        {
            printf("Writing to tun: ");
            print_header(packet);
            write_tun(packet, size);
            delete[] packet;
        }
    }
}

void *readInterface(void *arg)
{
    char buf[2048];

    std::cout << "Reading packages from tun interface" << std::endl;
    while (true)
    {
        // Sit in a loop, read a packet from fd, reflect
        // addresses and write back to fd
        ssize_t nread = read_tun(buf, sizeof(buf));
        //hex_dump(buf, nread);
        if (nread == 0)
            break;

        printf("Reading from tun: ");
        uint16_t len = print_header(buf);
        split_packet(buf, len, nullptr, &transmittBuffer, true);
    }
    return nullptr;
}

int main(int argc, char const *argv[])
{
    pthread_t writeThread;
    pthread_t readThread;
    pthread_t ctrlThread;
    pthread_t rxThread;
    pthread_t txThread;

    // define generic addresses
    uint8_t bsAddress[6] = "XBase";
    uint8_t myAddress[6] = "XNode"; //Change depending of this machines IP

    // change to device specific address:
    bsAddress[0] = id + '0';
    myAddress[0] = id + '0';

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

    setup("192.168.0.2/24"); //Change depending of this machines IP

    pthread_create(&writeThread, NULL, &writeInterface, NULL);
    pthread_create(&readThread, NULL, &readInterface, NULL);
    pthread_create(&ctrlThread, NULL, &controlThread, NULL);
    pthread_create(&rxThread, NULL, &reciveFragments, NULL);
    pthread_create(&txThread, NULL, &transmitterThread, NULL);

    pthread_join(writeThread, NULL);
    pthread_join(ctrlThread, NULL);
    pthread_join(rxThread, NULL);
    pthread_join(txThread, NULL);

    return 0;
}