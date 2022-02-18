#ifndef TUNMU_H
#define TUNMU_H

// thread worker functions
void* writeInterface(void* arg);
void* readInterface(void* arg);
void setup(std::string addr);

// utils
void print_header(char *buf);
void hex_dump(char *buff, uint16_t len);
int packet_len(char *buf);
#endif