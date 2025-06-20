#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <asm/termbits.h>  // struct termios2
#include <asm/ioctls.h>    // TCGETS2, TCSETS2
#include <time.h>


int fd485 = -1;

int rs485_set_termios2(int fd, uint32_t baud) {
    struct termios2 tio2;

    if (ioctl(fd, TCGETS2, &tio2) < 0) {
        perror("ioctl TCGETS2");
        return -1;
    }

    tio2.c_cflag &= ~CBAUD;
    tio2.c_cflag |= BOTHER;
    tio2.c_ispeed = baud;
    tio2.c_ospeed = baud;

    tio2.c_cflag &= ~(CSIZE | PARENB | CSTOPB);
    tio2.c_cflag |= CS8 | CLOCAL | CREAD;

    tio2.c_iflag = 0;
    tio2.c_oflag = 0;
    tio2.c_lflag = 0;

    tio2.c_cc[VMIN] = 1;
    tio2.c_cc[VTIME] = 0;

    if (ioctl(fd, TCSETS2, &tio2) < 0) {
        perror("ioctl TCSETS2");
        return -1;
    }

    printf("✅ RS485 port configured to %u bps\n", baud);
    return 0;
}

int rs485_init_2(const char *device_name, uint32_t baud) {
    fd485 = open(device_name, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd485 < 0) {
        perror("❌ Failed to open port");
        return -1;
    }

    if (rs485_set_termios2(fd485, baud) != 0) {
        close(fd485);
        fd485 = -1;
        return -1;
    }

    printf("✅ Opened RS485 device: %s\n", device_name);
    return 0;
}

int rs485_write_2(const void *buf, uint16_t len) {
    int written = write(fd485, buf, len);
    if (written < 0) {
        perror("❌ write error");
        return -1;
    }

    printf("<Write %d bytes>\n", written);
    for (int i = 0; i < written; ++i) {
        printf("%02X ", ((uint8_t *)buf)[i]);
        if ((i + 1) % 16 == 0) printf("\n");
    }
    if (written % 16 != 0) printf("\n");

    return written;
}

int rs485_read_2(void *buf, uint16_t buflen, double timeout_sec) {
    fd_set readfds;
    struct timeval timeout;
    int ret;

    FD_ZERO(&readfds);
    FD_SET(fd485, &readfds);

    timeout.tv_sec = (int)timeout_sec;
    timeout.tv_usec = (timeout_sec - timeout.tv_sec) * 1e6;

    ret = select(fd485 + 1, &readfds, NULL, NULL, &timeout);
    if (ret < 0) {
        perror("❌ select error");
        return -1;
    } else if (ret == 0) {
        printf("⏱️  read timeout (%.2f sec)\n", timeout_sec);
        return 0;
    }

    int rcv_len = read(fd485, buf, buflen);
    if (rcv_len < 0) {
        perror("❌ read error");
        return -1;
    }

    printf("<Read %d bytes>\n", rcv_len);
    for (int i = 0; i < rcv_len; ++i) {
        printf("%02X ", ((uint8_t *)buf)[i]);
        if ((i + 1) % 16 == 0) printf("\n");
    }
    if (rcv_len % 16 != 0) printf("\n");

    return rcv_len;
}

void rs485_fin_2(void) {
    if (fd485 >= 0) {
        close(fd485);
        fd485 = -1;
        printf("🔒 RS485 port closed\n");
    }
}