#ifndef FRAMES_HPP
#define FRAMES_HPP
/* Header file containg class represenations of our link layer frames */

enum CtrlType
{
    ack,
    propose,
    replyYes,
    replyNo,
};

// class Frame
// {
// public:
//     virtual char *serialize() = 0;
// };

// class DataFrame : Frame
// {
// public:
//     char *data;
//     int size;
//     int id;
//     int packet_num;
//     bool end;

// public:
//     DataFrame(char *buf);
//     char *serialize();
// };

class ControlFrame
{
public:
    CtrlType type;
    unsigned int ip;
    int time;

public:
    ControlFrame(char *buf);
    ControlFrame(CtrlType type, unsigned int ip, int time);
    char *serialize();
};

#endif