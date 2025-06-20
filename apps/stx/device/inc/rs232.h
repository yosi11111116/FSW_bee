#ifndef _RS232_H_
#define _RS232_H_
#include <inttypes.h>

int rs232_init(char * device_name, uint32_t baud);
int rs232_fin(void);
int rs232_read(void * buf, uint16_t bufsize);
int rs232_write(void * buf, uint16_t bufsize);

#endif