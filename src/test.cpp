#include <iostream>
#include <RF24/RF24.h>
#include <pthread.h>

/* PIN LAYOUT*/
#define CE_TX 17
#define CS_TX 10
#define CE_RX 27
#define CS_RX 0

#define PAYLOAD_SIZE 32

using namespace std;

RF24 radio_tx(CE_TX, CS_TX);
RF24 radio_rx(CE_RX, CS_RX);

char rx_payload[PAYLOAD_SIZE];
char tx_payload[PAYLOAD_SIZE];

void transmit() {
    radio_tx.stopListening();

    int failure = 0;
    tx_payload[0] = 'C';

    while (failure < 6)
    {
        bool delivered = radio_tx.write(&tx_payload, PAYLOAD_SIZE);

        if(delivered) {
            cout << "Transamission succesfull" << endl;
            cout << "Data sent: " << tx_payload << endl;
        } else {
            cout << "Transamission failure" << endl;
            failure++;
        }
        delay(1000);
    }
}

void* recive(void *data) 
{
  
    radio_rx.startListening();                                  // put radio in RX mode

    //TODO: Handle loop termination
    while (true) {
        if (radio_rx.available()) {                        // is there a payload? get the pipe number that recieved it
            radio_rx.read(&rx_payload, PAYLOAD_SIZE);                     // fetch payload from FIFO
            cout << "Received payload: " << (int) rx_payload[0] << endl;
        }
    }
    radio_rx.stopListening();
}

 int main(int argc, char const *argv[])
{
    uint8_t address[2][6] = {"0Node", "1Node"};
    
    
    // Start transmitter and reciver
    if(!radio_rx.begin()) {
        cout << "reciver is not responding!!" << endl;
        exit(1);
    }
    
    if(!radio_tx.begin()) {
        cout << "transmitter is not responding!!" << endl;
        exit(1);
    }

    // Transmitter config
    radio_tx.openWritingPipe(address[0]);
    radio_tx.setPALevel(RF24_PA_MIN);

    // Reciver config
    radio_rx.openReadingPipe(1, address[1]);
    radio_rx.setPALevel(RF24_PA_MIN);


    radio_rx.printDetails();
    radio_tx.printDetails();
    
    cout << "Starting threads" << endl;
    pthread_t rx_thread;
    pthread_create(&rx_thread, NULL, recive, NULL);
    transmit();

    return 0;
}