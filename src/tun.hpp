#ifndef TUNMU_H
#define TUNMU_H

// thread worker functions
void* startInterface(void* arg);
void* replyInterface(void* arg);

// utils
void print_header(char *buf);
void hex_dump(char *buff, uint16_t len);
#endif