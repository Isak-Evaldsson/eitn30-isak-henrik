#include <iostream>
#include <RF24/RF24.h>

using namespace std;

#define PAYLOAD_SIZE 32

// Radios
RF24 rxRadio(17, 0);
RF24 txRadio(27, 60);

// Payload buffers
char rxBuffer[PAYLOAD_SIZE];
char txBuffer[PAYLOAD_SIZE];

// function prototypes
void node0();
void node1();

int main()
{
    // perform hardware check
    if (!rxRadio.begin())
    {
        cout << "Reciver hardware is not responding!!" << endl;
        exit(1);
    }

    if (!txRadio.begin())
    {
        cout << "Transmitter hardware is not responding!!" << endl;
        exit(1);
    }

    // define addresses
    bool radioNumber = 1;
    uint8_t address[2][6] = {"0Node", "1Node"};

    // adress selection
    cout << "Which radio is this? Enter '0' or '1'. Defaults to '0' ";
    string input;
    getline(cin, input);
    radioNumber = input.length() > 0 && (uint8_t)input[0] == 49;

    // setup reciver
    rxRadio.setPayloadSize(PAYLOAD_SIZE);
    rxRadio.setPALevel(RF24_PA_LOW);
    rxRadio.openReadingPipe(1, address[radioNumber]);
    rxRadio.startListening();

    // setup transmitter
    txRadio.setPayloadSize(PAYLOAD_SIZE);
    txRadio.setPALevel(RF24_PA_LOW);
    txRadio.openWritingPipe(address[!radioNumber]);
    txRadio.stopListening();

    if (radioNumber)
    {
        node1();
    }
    else
    {
        node0();
    }
}

void node0()
{
    string msg;

    while (true)
    {
        cout << "Enter your name: " << endl;
        getline(cin, msg);
        strcpy(txBuffer, msg.c_str()); // Will buffer overflow
        bool succ = txRadio.write(txBuffer, PAYLOAD_SIZE);

        if (!succ)
        {
            cout << "Transamission failed" << endl;
            continue;
        }

        while (true)
        {
            if (rxRadio.available())
            {
                rxRadio.read(rxBuffer, PAYLOAD_SIZE);
                cout << "Received apply: " << rxBuffer << endl;
                break;
            }
        }
    }
}

void node1()
{
    while (true)
    {
        if (rxRadio.available())
        {
            rxRadio.read(rxBuffer, PAYLOAD_SIZE);
            cout << "Received: " << rxBuffer << endl;

            strcpy(txBuffer, "hello");
            memcpy(txBuffer + 5, rxBuffer, 27);
            bool delivered = txRadio.write(&txBuffer, PAYLOAD_SIZE);

            if (delivered)
            {
                cout << "Transamission succesfull " << endl;
                break;
            }
            else
            {
                cout << "Transamission failure" << endl;
            }
        }
    }
}