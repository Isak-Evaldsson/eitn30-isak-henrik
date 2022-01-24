#include <iostream>
#include <string>
#include <RF24/RF24.h>

using namespace std;

RF24 radio(17, 0);
unsigned int num;

void recive() 
{

    radio.startListening();                                  // put radio in RX mode

    //TODO: Handle loop termination
    while (true) {
        uint8_t pipe;

        if (radio.available(&pipe) && pipe == 1) {
            cout << (unsigned int) radio.getPayloadSize() << endl;                        // is there a payload? get the pipe number that recieved it
            radio.read(&num, radio.getPayloadSize());
                                          // fetch payload from FIFO
            cout << "Received payload: " << num << endl;
        }
    }
    radio.stopListening();
}

void init_radio() {
    uint8_t address[6] = "0Node";


    if(!radio.begin()) {
        cout << "reciver is not responding!!" << endl;
        exit(1);
    }

    radio.openReadingPipe(1, address);
    radio.setPALevel(RF24_PA_MIN);
    radio.setPayloadSize(sizeof(int));
}   

int main() {
    init_radio();

    recive();
}