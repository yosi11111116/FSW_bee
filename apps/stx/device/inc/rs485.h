#ifndef _RS485_H_
#define _RS485_H_
#include <inttypes.h>
// #ifndef _RS232_h_
// #include <rs232.h>
// #endif

int rs485_init_2(char * device_name, uint32_t baud);
int rs485_fin_2(void);
int rs485_read_2(void * buf, uint16_t bufsize);
int rs485_write_2(void * buf, uint16_t bufsize);
// int rs485_scan_2(void * buf, uint16_t bufsize);
int rs485_set_baud_custom(int fd, uint32_t baud); 

#define RS485_INIT  rs485_init_2
#define RS485_FIN   rs485_fin_2
#define RS485_READ  rs485_read_2
#define RS485_WRITE rs485_write_2

#endif