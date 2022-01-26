#include <iostream>
#include <string>
#include <RF24/RF24.h>

using namespace std;

RF24 radio(17, 0);

void init_radio()
{
    uint8_t address[6] = "0Node";

    if (!radio.begin())
    {
        cout << "transmitter is not responding!!" << endl;
        exit(1);
    }

    radio.openWritingPipe(address);
    radio.setPALevel(RF24_PA_MIN);
    radio.setPayloadSize(sizeof(int));
}

void transmitt(unsigned int num)
{
    radio.stopListening();
    int failure = 0;

    while (failure < 15)
    {
        bool delivered = radio.write(&num, sizeof(int));

        if (delivered)
        {
            cout << "Transamission succesfull, sent: " << num << endl;
            break;
        }
        else
        {
            cout << "Transamission failure" << endl;
            failure++;
        }
        delay(1000);
    }
}

int main()
{
    string msg;
    init_radio();

    while (getline(cin, msg))
    {
        cout << "Payload size:" << (unsigned int)radio.getPayloadSize() << endl;
        transmitt(stoi(msg));
    }
}