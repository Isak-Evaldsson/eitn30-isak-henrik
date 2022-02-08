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

#define CHECKAUX(e, s) \
    ((e) ? (void)0 : (fprintf(stderr, "'%s' failed at %s:%d - %s\n", s, __FILE__, __LINE__, strerror(errno)), exit(0)))
#define CHECK(e) (CHECKAUX(e, #e))
#define CHECKSYS(e) (CHECKAUX((e) == 0, #e))
#define CHECKFD(e) (CHECKAUX((e) >= 0, #e))
#define STRING(e) #e

// Prototypes
void reflect(uint8_t *buf, ssize_t size);
void print_header(uint8_t *buf);

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

int main(int argc, char *argv[])
{
    char dev[IFNAMSIZ + 1]; // array containg tun device name

    memset(dev, 0, sizeof(dev));
    if (argc > 1)
        strncpy(dev, argv[1], sizeof(dev) - 1);

    // Allocate the tun device
    int fd = tun_alloc(dev);
    if (fd < 0)
        exit(0);

    uint8_t buf[2048];

    std::cout << "Reading packages from tun interface" << std::endl;
    while (true)
    {
        // Sit in a loop, read a packet from fd, reflect
        // addresses and write back to fd
        ssize_t nread = read(fd, buf, sizeof(buf));
        CHECK(nread >= 0);
        if (nread == 0)
            break;

        print_header(buf);
        reflect(buf, nread);
        size_t nwrite = write(fd, buf, nread);
        CHECK(nwrite == nread);
    }
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

// Switches src and dest in ip header
void reflect(uint8_t *buf, ssize_t size)
{
    uint8_t version = buf[0] >> 4; // right-shift to remove ihl field
    switch (version)
    {
    case 4:
        break;
    case 6:
        fprintf(stderr, "IPv6 not implemented yet, ignores the packages\n");
        return;
    default:
        fprintf(stderr, "Unknown protocol %u\n", version);
        exit(0);
    }

    uint32_t src = get32(buf, 12);
    uint32_t dst = get32(buf, 16);
    put32(buf, 12, dst);
    put32(buf, 16, src);
}

void print_header(uint8_t *buf)
{
    // Extracting header fields
    uint8_t version = buf[0] >> 4;
    uint8_t ihl = buf[0] & 0b1111;
    uint8_t protocol = buf[9];

    uint16_t lenght;
    memcpy(&lenght, buf + 2, 2);
    uint32_t src = get32(buf, 12);
    uint32_t dst = get32(buf, 16);

    printf("Recived packet, version: %d, ihl: %d, protocol: %d, length: %+" PRIu16 "", version, ihl, protocol, lenght);
    printf(" src: %d.%d.%d.%d, dst: %d.%d.%d.%d\n", buf[12], buf[13], buf[14], buf[15], buf[16], buf[17], buf[18], buf[19]);
}