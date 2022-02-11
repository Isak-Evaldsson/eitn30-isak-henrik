#ifndef TUNMU_H
#define TUNMU_H

// thread worker functions
void* startInterface(void* arg);
void* replyInterface(void* arg);

// utils
void print_header(uint8_t *buf);
#endif