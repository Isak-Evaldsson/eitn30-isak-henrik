#include <iostream>
#include <string>
#include <RF24/RF24.h>

using namespace std;

RF24 radio(27, 10);

void init_radio() {
    uint8_t address[6] = "ONode";


    if(!radio.begin()) {
        cout << "transmitter is not responding!!" << endl;
        exit(1);
    }

    radio.openWritingPipe(address);
    radio.setPALevel(RF24_PA_MIN);
}

void transmitt(string msg) {
    radio.stopListening();
    int failure = 0;

    while (failure < 6)
    {
        bool delivered = radio.write(msg.c_str(), msg.size());

        if(delivered) {
            cout << "Transamission succesfull" << endl;
        } else {
            cout << "Transamission failure" << endl;
            failure++;
        }
        delay(1000);
    }
}


int main() {
    string msg;
    init_radio();
   
    while (getline(cin, msg))
    {
        cout << "Sending: " << msg << endl;
        transmitt(msg);
    }
}