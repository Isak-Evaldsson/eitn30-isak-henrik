#ifndef TUNMU_H
#define TUNMU_H
#include "transmittBuffer.hpp"

void setup(std::string addr);
uint16_t print_header(char *buf);
void write_tun(char* packet, int size);
ssize_t read_tun(char* buf, size_t s);
void hex_dump(char *buff, uint16_t len);
int packet_len(char *buf);
void split_packet(char *buf, uint16_t lenght, std::map<unsigned int, TransmittBuffer>* transmittMap, TransmittBuffer* transBuffer, bool singleBuffer);

#endif