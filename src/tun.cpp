#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <inttypes.h>
#include <iostream>
#include <map>

// Linux headers/ Syscalls
#include <linux/if_tun.h>
#include <linux/if.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "tun.hpp"
#include "fragmentBuffer.hpp"
#include "frames.hpp"

#define CHECKAUX(e, s) \
    ((e) ? (void)0 : (fprintf(stderr, "'%s' failed at %s:%d - %s\n", s, __FILE__, __LINE__, strerror(errno)), exit(0)))
#define CHECK(e) (CHECKAUX(e, #e))
#define CHECKSYS(e) (CHECKAUX((e) == 0, #e))
#define CHECKFD(e) (CHECKAUX((e) >= 0, #e))
#define STRING(e) #e

// Global vars
static int tun_fd = -1; // Contains the file descritor id used to read and write to tun

int tun_alloc(char *dev)
{
    assert(dev != NULL);
    int fd = open("/dev/net/tun", O_RDWR);
    CHECKFD(fd);

    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
    strncpy(ifr.ifr_name, dev, IFNAMSIZ);
    CHECKSYS(ioctl(fd, TUNSETIFF, (void *)&ifr));
    strncpy(dev, ifr.ifr_name, IFNAMSIZ);

    printf("Created tun device %s\n", dev);
    return fd;
}

// Calls appropraite bash commands to setup the tun device correctly
void setup(std::string addr) {
    char dev[IFNAMSIZ + 1]; // array containg tun device name
    char buf[2048];

    memset(dev, 0, sizeof(dev));

    // Allocate the tun device
    tun_fd = tun_alloc(dev);
    if (tun_fd < 0) {
        printf("Could not create tun device");
        exit(0);
    }

    // Appropritate commands to setup device
    std::string command = "sudo ip addr add " + addr + " dev tun0";

    system("sudo ip link set tun0 up");
    system(command.c_str());
}

void write_tun(char* packet, int size) {
    write(tun_fd, packet, size);  
}

ssize_t read_tun(char* buf, size_t s) {
    return read(tun_fd, buf, s);
}

uint16_t print_header(char *buf)
{
    // Extracting header fields
    uint8_t version = buf[0] >> 4;
    uint8_t ihl = buf[0] & 0b1111;
    uint8_t protocol = buf[9];

    // Computes length by adding the two bytes within the header
    uint16_t lenght = buf[2];
    lenght = lenght << 8;
    lenght += buf[3];

    
    int icmp1 = buf[22];
    int icmp2 = buf[23];
    int icmp_check = (buf[22] << 8)  + buf[23];
    int tcp_hdr_len = buf[32] >> 4;
    int icmp_seq = (buf[26] << 8) + buf[27];
    printf("IP packet, version: %d, ihl: %d, protocol: %d, length: %+" PRIu16"", version, ihl, protocol, lenght);
    printf(" src: %d.%d.%d.%d, dst: %d.%d.%d.%d", buf[12], buf[13], buf[14], buf[15], buf[16], buf[17], buf[18], buf[19]);
    printf(" tcp hdr lenght %d", tcp_hdr_len);
    printf(" icmp seq nbr: %d", icmp_seq);
    printf(" icmp checksum %x\n", icmp_check);

    return lenght;
}

void split_packet(char *buf, uint16_t lenght, std::map<unsigned int, TransmittBuffer>* transmittMap, TransmittBuffer* transBuffer, bool singleBuffer) {
    // only split IPv4 packets
    if (buf[0] >> 4 != 4) 
        return;


    int id = rand() % 512;
    int nbr_full_packets = lenght / 30;
    int len_last_packet = lenght % 30;
    int nbr_packets = nbr_full_packets + 1;
    unsigned int dst;

    // If last packets is full, modolus will result in 0
    if(len_last_packet == 0) {
        nbr_packets = nbr_full_packets;
        len_last_packet = 30; 
    }

    if (!singleBuffer) {
        dst =  (buf[19]<<0) | (buf[18]<<8) | (buf[17]<<16) | (buf[16]<<24);
    }

    for (int i = 0; i < nbr_packets; i++) {
        // sets length depeding if last packet
        int len = i == nbr_packets - 1 ? len_last_packet : 30;
        
        // Copy part of ip-packet
        char* data = (char*) calloc(len, sizeof(char));
        memcpy(data, buf + (i * 30), sizeof(char) * len);

        DataFrame* item = new DataFrame(data, len, id, i, i == nbr_packets - 1);
        //hex_dump(item->data, item->size);
        
        if(!singleBuffer) {
            transmittMap->at(dst).pushDataFrame(item);
        } else {
            transBuffer->pushDataFrame(item);
        }
    }

    printf("Packet spit into %d fragments\n", nbr_packets);
}

unsigned int get_dest(char *buff) {
    return (buff[19]<<0) | (buff[18]<<8) | (buff[17]<<16) | (buff[16]<<24);
}

void hex_dump(char *buff, uint16_t len) {
    for (size_t i = 0; i < len; i++)
    {
        printf("%02x ", buff[i]);
    }
    
    printf("\n");
    
}

int packet_len(char *buf) {
    int lenght = buf[2];
    lenght = lenght << 8;
    lenght += buf[3];

    return lenght;
}