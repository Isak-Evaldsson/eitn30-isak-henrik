#include <ctime>       // time()
#include <iostream>    // cin, cout, endl
#include <string>      // string, getline()
#include <time.h>      // CLOCK_MONOTONIC_RAW, timespec, clock_gettime()
#include <RF24/RF24.h> // RF24, RF24_PA_LOW, delay()

using namespace std;

#define PAYLOAD_SIZE 32

// Radio
RF24 radio(17, 0);

// Messages sent by transmitter
char message[PAYLOAD_SIZE] = "Testar testar testar";

// Data stores buy reciver
char reciveBuffer[PAYLOAD_SIZE];

// Function prototypes
void transmitter();
void reciver();
void setRole();

int main(int argc, char **argv)
{

    // perform hardware check
    if (!radio.begin())
    {
        cout << "radio hardware is not responding!!" << endl;
        return 0; // quit now
    }

    radio.setChannel(107);
    radio.setDataRate(RF24_2MBPS);

    // to use different addresses on a pair of radios, we need a variable to
    // uniquely identify which address this radio will use to transmit
    bool radioNumber = 1; // 0 uses address[0] to transmit, 1 uses address[1] to transmit

    // Let these addresses be used for the pair
    uint8_t address[2][6] = {"1Node", "2Node"};
    // It is very helpful to think of an address as a path instead of as
    // an identifying device destination

    // Set the radioNumber via the terminal on startup
    cout << "Which radio is this? Enter '0' or '1'. Defaults to '0' ";
    string input;
    getline(cin, input);
    radioNumber = input.length() > 0 && (uint8_t)input[0] == 49;

    // save on transmission time by setting the radio to only transmit the
    // number of bytes we need to transmit a float
    radio.setPayloadSize(PAYLOAD_SIZE); // float datatype occupies 4 bytes

    // Set the PA Level low to try preventing power supply related problems
    // because these examples are likely run with nodes in close proximity to
    // each other.
    radio.setPALevel(RF24_PA_LOW); // RF24_PA_MAX is default.

    // set the TX address of the RX node into the TX pipe
    radio.openWritingPipe(address[radioNumber]); // always uses pipe 0

    // set the RX address of the TX node into a RX pipe
    radio.openReadingPipe(1, address[!radioNumber]); // using pipe 1
    radio.closeReadingPipe(3);

    setRole(); // calls master() or slave() based on user input
    return 0;
}

void setRole()
{
    string input = "";
    while (!input.length())
    {
        cout << "*** PRESS 'T' to begin transmitting to the other node\n";
        cout << "*** PRESS 'R' to begin receiving from the other node\n";
        cout << "*** PRESS 'Q' to exit" << endl;
        getline(cin, input);
        if (input.length() >= 1)
        {
            if (input[0] == 'T' || input[0] == 't')
                transmitter();
            else if (input[0] == 'R' || input[0] == 'r')
                reciver();
            else if (input[0] == 'Q' || input[0] == 'q')
                break;
            else
                cout << input[0] << " is an invalid input. Please try again." << endl;
        }
        input = ""; // stay in the while loop
    }
}

void transmitter()
{
    int benchmarkTime = 60;
    int nbrSent = 0;
    int nbrFailed = 0;

    radio.stopListening();
    time_t startTime = time(nullptr);

    cout << "Begin packet transmission for " << benchmarkTime << " seconds:" << endl;

    // Loop for one minute
    while (time(nullptr) - startTime < benchmarkTime)
    {
        bool success = radio.write(&message, PAYLOAD_SIZE);
        nbrSent++;
        if (!success)
            nbrFailed++;
    }

    float bandwidth = ((nbrSent - nbrFailed) * PAYLOAD_SIZE * 8) / (benchmarkTime * 1000);

    cout << "Benchmark Complete:" << endl;
    cout << "Number of sent packages: " << nbrSent << endl;
    cout << "Number of failed transmission: " << nbrFailed << endl;
    cout << "Bandwidth: " << bandwidth << " kbps" << endl
         << endl;
}

void reciver()
{

    radio.startListening(); // put radio in RX mode

    //TODO: Handle loop termination
    while (true)
    {
        uint8_t pipe;

        if (radio.available(&pipe))
        {
            // is there a payload? get the pipe number that recieved it
            radio.read(reciveBuffer, PAYLOAD_SIZE);
            cout << "Received payload: " << reciveBuffer << endl;
        }
    }
    radio.stopListening();
}