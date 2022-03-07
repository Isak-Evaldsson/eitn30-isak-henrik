#ifndef TUNMU_H
#define TUNMU_H
#include "transmittBuffer.hpp"

void setup(std::string addr);
void write_tun(char* packet, int size);
void hex_dump(char *buff, uint16_t len);
void split_packet(char *buf, uint16_t lenght, std::map<unsigned int, TransmittBuffer>* transmittMap, TransmittBuffer* transBuffer, bool singleBuffer);
int packet_len(char *buf);
uint16_t print_header(char *buf);
ssize_t read_tun(char* buf, size_t s);
unsigned int get_dest(char *buff);

#endif