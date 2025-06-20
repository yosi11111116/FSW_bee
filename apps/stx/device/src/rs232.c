#include <stdio.h>
#include <fcntl.h>   	 /* File Control Definitions           */
#include <termios.h> 	 /* POSIX Terminal Control Definitions */
#include <unistd.h>  	 /* UNIX Standard Definitions 	       */ 
#include <errno.h>
#include <inttypes.h>
#include <sys/ioctl.h> 
#include <linux/serial.h>
#include <rs232.h>

int fd232 = 1;
struct termios TMS232;

static void rs232_set_baud(uint32_t baud)
{
    printf("Set Baudrate : %u\n", baud);
    switch(baud) {
    case 4800 : {
        cfsetispeed(&TMS232, B4800);
        cfsetospeed(&TMS232, B4800);
        break;
    }
    case 9600 : {
        cfsetispeed(&TMS232, B9600);
        cfsetospeed(&TMS232, B9600);
        break;
    }
    case 19200 : {
        cfsetispeed(&TMS232, B19200);
        cfsetospeed(&TMS232, B19200);
        break;
    }
    case 38400 : {
        cfsetispeed(&TMS232, B38400);
        cfsetospeed(&TMS232, B38400);
        break;
    }
    case 1000000 : {
        cfsetispeed(&TMS232, B1000000);
        cfsetospeed(&TMS232, B1000000);
        break;
    }
    case 3000000 : {
        cfsetispeed(&TMS232, B3000000);
        cfsetospeed(&TMS232, B3000000);
        break;
    }
    default: {
        cfsetispeed(&TMS232, B4800);
        cfsetospeed(&TMS232, B4800);
        break;
    }
    }
}

int rs232_init(char * device_name, uint32_t baud)
{
    int ret = -1;

    fd232 = open(device_name, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
    if(fd232 < 0)
    {
        printf("Error! in Opening ttyUSB0. fd232 : %d\n", fd232);
        return fd232;
    }
    else
        printf("rs232 Opened Successfully. fd232 : %d\n", fd232);

    
    ret = tcgetattr(fd232, &TMS232);
    if(ret != 0)
    {
        printf("Error getting serial port settings. Retcode : %d\n", ret);
        return ret;
    }
    else
        printf("rs232 Setting Loaded.\n");
    
    rs232_set_baud(baud);

    TMS232.c_cflag &= ~PARENB;
    TMS232.c_cflag &= ~CSTOPB;
    TMS232.c_cflag &= ~CSIZE;
    TMS232.c_cflag |= CS8;

    if (tcsetattr(fd232, TCSANOW, &TMS232) != 0) {
        printf("Error setting serial port settings.\n");
        rs232_fin();
        return -1;
    }
    return 0;
}

int rs232_fin(void)
{
    tcflush(fd232, TCIOFLUSH);
    return close(fd232);
}

int rs232_read(void * buf, uint16_t bufsize)
{
    int len = 0;
    printf("Read Start...\n");
    len = read(fd232, buf, bufsize);
    printf("Bytes Rxed : %d\n", len); /* Print the number of bytes read */
    char* read_buf = (char*) buf;
    if(len <= 0)
        return len;
    for(int i = 0; i < len; i++)	 /*printing only the received characters*/
    {
        printf("%hhx\t",read_buf[i]);
        if(i%10 == 9)
            printf("\n");
    }
    printf("\n");

    tcflush(fd232, TCIFLUSH);
    return len;
}

int rs232_write(void * buf, uint16_t bufsize)
{
    int len = 0;
    printf("Write Start...\n");
    len = write(fd232, buf, bufsize);
    printf("Bytes Txed : %d\n", len); /* Print the number of bytes read */
    char * read_buf = (char*) buf;
    if(len <= 0)
        return len;
    for(int i = 0;i < len;i++)	 /*printing only the received characters*/
    {
        printf("%hhx\t",read_buf[i]);
        if(i%10 == 9)
            printf("\n");
    }
    printf("\n");

    tcflush(fd232, TCOFLUSH);
    return len;
}