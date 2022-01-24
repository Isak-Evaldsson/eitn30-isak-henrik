#include <iostream>
#include <string>
#include <RF24/RF24.h>

using namespace std;

RF24 radio(27, 10);

void recive() 
{
  char string[32];

    radio.startListening();                                  // put radio in RX mode

    //TODO: Handle loop termination
    while (true) {
        if (radio.available()) {                        // is there a payload? get the pipe number that recieved it
            radio.read(&string, 32);                     // fetch payload from FIFO
            string[31] = '\0';
            cout << "Received payload: " << string << endl;
        }
    }
    radio.stopListening();
}

void init_radio() {
    uint8_t address[6] = "1Node";


    if(!radio.begin()) {
        cout << "reciver is not responding!!" << endl;
        exit(1);
    }

    radio.openReadingPipe(1, address);
    radio.setPALevel(RF24_PA_MIN);
}

int main() {
    init_radio();

    recive();
}