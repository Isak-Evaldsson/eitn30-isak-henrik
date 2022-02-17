#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <inttypes.h>
#include <iostream>

// Linux headers/ Syscalls
#include <linux/if_tun.h>
#include <linux/if.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "transmittBuffer.hpp"
#include "fragmentBuffer.hpp"

#define CHECKAUX(e, s) \
    ((e) ? (void)0 : (fprintf(stderr, "'%s' failed at %s:%d - %s\n", s, __FILE__, __LINE__, strerror(errno)), exit(0)))
#define CHECK(e) (CHECKAUX(e, #e))
#define CHECKSYS(e) (CHECKAUX((e) == 0, #e))
#define CHECKFD(e) (CHECKAUX((e) >= 0, #e))
#define STRING(e) #e

// Prototypes
void reflect(uint8_t *buf, ssize_t size);
uint16_t print_header(uint8_t *buf);
void split_packet(uint8_t *buf, uint16_t lenght);
void extractHeader(uint8_t *buf, ssize_t size);

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
void setup() {
    system("sudo ip link set tun0 up");
    system("sudo ip addr add 192.168.0.1/24 dev tun0");
}

void* startInterface(void* arg)
{
    char dev[IFNAMSIZ + 1]; // array containg tun device name
    uint8_t buf[2048];

    memset(dev, 0, sizeof(dev));

    // Allocate the tun device
    int fd = tun_alloc(dev);
    if (fd < 0)
        exit(0);

    setup();

    std::cout << "Reading packages from tun interface" << std::endl;
    while (true)
    {
        // Sit in a loop, read a packet from fd, reflect
        // addresses and write back to fd
        ssize_t nread = read(fd, buf, sizeof(buf));
        CHECK(nread >= 0);
        if (nread == 0)
            break;

        uint16_t len = print_header(buf);
        split_packet(buf, len);
        //extractHeader(buf, nread);
    }
    return nullptr;
}

void* replyInterface(void* arg)
{
    char dev[IFNAMSIZ + 1]; // array containg tun device name

    memset(dev, 0, sizeof(dev));

    // Allocate the tun device
    int fd = tun_alloc(dev);
    if (fd < 0)
        exit(0);

    setup();
    std::cout << "writing packages to tun interface" << std::endl;

    while (true)
    {
       int id = getNextId();
       
       if (id != -1)
       {
            int size;
            uint8_t* packet = createPacket(id, &size);
            print_header(packet);

            // Print out packet to file
            FILE *f = fopen("packet_bs", "wb");
            fwrite(packet, sizeof(uint8_t), size, f);
            fclose(f);

            std::cout << "Writing packet to tun, size: " << size << std::endl;
            write(fd, packet, size);
            delete[] packet;
       }
    }

    return nullptr;
} 

void extractHeader(uint8_t *buf, ssize_t size)
{
    char* header = (char*) malloc(sizeof(char) * 20);
    memcpy(header, buf, sizeof(char) * 20);
    pushBufferItem(header, sizeof(char) * 20);
}

static void put16(uint8_t *p, size_t offset, uint16_t n)
{
    memcpy(p + offset, &n, sizeof(n));
}

static uint16_t get16(uint8_t *p, size_t offset)
{
    uint16_t n;
    memcpy(&n, p + offset, sizeof(n));
    return n;
}

static void put32(uint8_t *p, size_t offset, uint32_t n)
{
    memcpy(p + offset, &n, sizeof(n));
}

static uint32_t get32(uint8_t *p, size_t offset)
{
    uint32_t n;
    memcpy(&n, p + offset, sizeof(n));
    return n;
}

uint16_t print_header(uint8_t *buf)
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

    printf("Recived packet, version: %d, ihl: %d, protocol: %d, length: %+" PRIu16"", version, ihl, protocol, lenght);
    printf(" src: %d.%d.%d.%d, dst: %d.%d.%d.%d", buf[12], buf[13], buf[14], buf[15], buf[16], buf[17], buf[18], buf[19]);
    printf(" tcp hdr lenght %d", tcp_hdr_len);
    printf(" icmp checksum %x\n", icmp_check);

    return lenght;
}

void split_packet(uint8_t *buf, uint16_t lenght) {
    // only split IPv4 packets
    if (buf[0] >> 4 != 4) 
        return;

    int len_last_packet = lenght % 30;
    int nbr_full_packets = lenght / 30;
    int nbr_packets = len_last_packet == 0 ? nbr_full_packets : nbr_full_packets + 1;
    int id = rand() % 1024;

    for (int i = 0; i < nbr_packets; i++) {
        // sets length depeding if last packet
        int len = i == nbr_packets - 1 ? len_last_packet : 30;
        
        // Copy part of ip-packet
        char* data = (char*) calloc(len, sizeof(char));
        memcpy(data, buf + (i * 30), sizeof(char) * len);

        BufferItem* item = new BufferItem(data, len, id, i, i == nbr_packets - 1);
        pushBufferItem(item);
    }

    printf("Packet spit into %d fragments\n", nbr_packets);
}