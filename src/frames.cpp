#include "frames.hpp"
#include <cstring>


DataFrame::DataFrame(char *buf)
{
    // Extracting fields from buffer
    packet_num = buf[0] & 0x3e;
    packet_num >>= 1;

    id = buf[0] & 0x01;
    id <<= 8;
    id += buf[1];

    end = buf[0] & 0x40;
    size = 30; // can maybe be removed

    // Copy our data field
    data = new char[32]; //maybe only copy the package contents
    std::memcpy(data, buf, 32);
}

DataFrame::DataFrame(char* data, int size, int id, int packet_num, bool end)
{
    this->data = data;
    this->size = size;
    this->id = id;
    this->packet_num = packet_num;
    this->end = end;
}

char* DataFrame::serialize()
{
    char *frame = new char[32]();
    
    memcpy(frame + 2, data, 30);
    frame[1] = id & 0xff;
    frame[0] = (id >> 8) & 0x01;
    frame[0] |= (packet_num << 1) & 0x3e;

    // setting end bit
    if(end) {
        frame[0] |= 0x40;
    }

    return frame;
}


ControlFrame::ControlFrame(char *buf)
{
    int tmp = buf[0] & 0x7f;

    switch (tmp)
    {
    case 0:
        type = ack;
        break;
    case 1:
        type = propose;
        break;
    case 2:
        type = replyYes;
        break;
    case 3:
        type = replyNo;
        break;
    }

    std::memcpy(&ip, buf + 1, 4);
    std::memcpy(&time, buf + 5, 2);
}

ControlFrame::ControlFrame(CtrlType type, unsigned int ip, int time)
{
    this->type = type;
    this->ip = ip;
    this->time = time;
}

char *ControlFrame::serialize()
{
    char *frame = new char[32];

    switch (type)
    {
    case ack:
        frame[0] = 0;
        break;
    case propose:
        frame[0] = 1;
        break;
    case replyYes:
        frame[0] = 2;
        break;
    case replyNo:
        frame[0] = 3;
        break;
    }

    frame[0] |= 0x80; // Set controlbit to 1

    std::memcpy(frame + 1, &ip, 4);
    std::memcpy(frame + 5, &time, 4);

    return frame;
}