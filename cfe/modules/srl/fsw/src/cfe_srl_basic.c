/************************************************************************
 * Author : Hyeokjin Kweon
 * 
 * Last Modified : 2025 - 04 - 27
 * 
 * Brief : Serial Comm. Core Module's Event ID. 
 *         Do NOT use this function directly in App layer
 ************************************************************************/
#include <fcntl.h>

#include <sys/select.h>

#include <poll.h>


#include "cfe_srl_basic.h"

int CFE_SRL_BasicOpen(const char *Device, int Option) {
    return open(Device, Option);
}

ssize_t CFE_SRL_BasicWrite(int FD, const void *Data, size_t Size) {
    return write(FD, Data, Size);
}

ssize_t CFE_SRL_BasicRead(int FD, void *Data, size_t Size) {
    return read(FD, Data, Size);
}

int CFE_SRL_BasicClose(int FD) {
    return close(FD);
}

int CFE_SRL_BasicIOCTL(int FD, unsigned long Request, void *Data) {
    uint8_t Arg;

    if (Request == I2C_SLAVE) {
        Arg = *(uint8_t *)Data;
        return ioctl(FD, Request, Arg);
    }
    else if (Request == I2C_RDWR) {
        return ioctl(FD, Request, (struct i2c_rdwr_ioctl_data *)Data);
    }
    else return ioctl(FD, Request, Data);
}

ssize_t CFE_SRL_BasicPollRead(int FD, void *Data, size_t Size, uint32_t Timeout) {
    // Better efficiency than `select()`
    struct pollfd FDS[1];

    FDS[0].fd = FD;
    FDS[0].events = POLLIN; // Read Events

    int Ret = poll(FDS, 1, Timeout);

    if (Ret > 0 && FDS[0].revents & POLLIN) { // When Read Event Occured,
        // int BufSize = 0;
        // uint8_t Iteration = 0;
        // while (BufSize < Size) {
        //     if (Iteration > 5) break;
        //     OS_TaskDelay(1);
        //     ioctl(FD, FIONREAD, &BufSize);
        //     OS_printf("Received Data : %d\n",BufSize);
        //     Iteration++;
        // }
        return CFE_SRL_BasicRead(FD, Data, Size); // Read.
    }
    else if (Ret == 0) {
        return CFE_SRL_TIMEOUT;
    }
    else {
        return CFE_SRL_ERR;
    }
}


CFE_SRL_DevType_t CFE_SRL_GetHandleDevType(CFE_SRL_IO_Handle_t *Handle) {
    if (Handle == NULL) return CFE_SRL_BAD_ARGUMENT;

    CFE_SRL_Global_Handle_t *Entry = (CFE_SRL_Global_Handle_t *)Handle;
    
    return Entry->DevType;
}

bool CFE_SRL_QueryStatus(const CFE_SRL_Global_Handle_t *Entry, uint8_t Query) {
    return (Entry->Status & Query) == Query;
}

int CFE_SRL_SetHandleStatus(CFE_SRL_IO_Handle_t *Handle, uint8_t Label, bool Set) {
    if (Handle == NULL) return CFE_SRL_BAD_ARGUMENT;
    CFE_SRL_Global_Handle_t *Entry = (CFE_SRL_Global_Handle_t *)Handle;

    if (Set == true) Entry->Status |= Label;
    else Entry->Status &= ~Label;

    return CFE_SUCCESS;
}

/*************************************************************
 * 
 *  Basic UART/RS422 Function
 * 
 *************************************************************/
<<<<<<< HEAD
/**
 * Deprecated
 */ 
speed_t CFE_SRL_GetBaudFromInt(uint32_t BaudRate) {
    switch(BaudRate) {
    //     case 2400: return B2400;
    //     case 4800: return B4800;
    //     case 9600: return B9600;
    //     case 19200: return B19200;
    //     case 38400: return B38400;
    //     case 57600: return B576000;
    //     case 115200: return B115200;
    //     case 230400: return B230400;
    //     case 460800: return B460800;
    //     case 500000: return B500000;

        default:
            return -1;
    }
}

/**
 * Deprecated
 */
int CFE_SRL_GetTermiosAttr(CFE_SRL_IO_Handle_t *Handle, struct termios *Termios) {
    // int Status;
    // CFE_SRL_DevType_t DevType;

    // if (Handle == NULL || Termios == NULL ) return CFE_SRL_BAD_ARGUMENT;

    // DevType = CFE_SRL_GetHandleDevType(Handle);
    // if (DevType != SRL_DEVTYPE_UART && DevType != SRL_DEVTYPE_RS422) {
    //     return -1; // Revise to `TYPE_NOT_UART`
    // }

    // Status = CFE_SRL_QueryStatus((CFE_SRL_Global_Handle_t *)Handle, CFE_SRL_HANDLE_STATUS_FD_INIT);
    // if (Status == false) return CFE_SRL_NOT_OPEN_ERR;

    // Status = tcgetattr(Handle->FD, Termios);
    // if (Status < 0) {
    //     Handle->__errno = errno;
    //     return -1; // Revise to `UART_GET_ATTR_ERR`
    // }

    return CFE_SUCCESS;
}

/**
 * Deprecated
 */
int CFE_SRL_SetTermiosAttr(CFE_SRL_IO_Handle_t *Handle, struct termios *Termios) {
    // int Status;
    // CFE_SRL_DevType_t DevType;

    // if (Handle == NULL || Termios == NULL) return CFE_SRL_BAD_ARGUMENT;

    // DevType = CFE_SRL_GetHandleDevType(Handle);
    // if (DevType != SRL_DEVTYPE_UART && DevType != SRL_DEVTYPE_RS422) {
    //     return -1; // Revise to `TYPE_NOT_UART`
    // }

    // Status = CFE_SRL_QueryStatus((CFE_SRL_Global_Handle_t *)Handle, CFE_SRL_HANDLE_STATUS_FD_INIT);
    // if (Status == false) return CFE_SRL_NOT_OPEN_ERR;

    // Status = tcsetattr(Handle->FD, TCSANOW, Termios);
    // if (Status < 0) {
    //     Handle->__errno = errno;
    //     return -1; // Revise to `UART_SET_ATTR_ERR`
    // }

    return CFE_SUCCESS;
}

/**
 * Deprecated
 * Module use `CFE_SRL_BasicSetUART2` instead
 */
=======
>>>>>>> 9da93d5463f7574bd99ace62fa6f91688270af48
int CFE_SRL_BasicSetUART(CFE_SRL_IO_Handle_t *Handle, uint32_t BaudRate) {
    // int Status;
    // struct termios Termios;
    // speed_t Baud;

    // if (Handle == NULL) return CFE_SRL_BAD_ARGUMENT;

    // Baud = CFE_SRL_GetBaudFromInt(BaudRate);
    // if (Baud < 0) return -1; // Revise to `CFE_SRL_INVALID_BAUD`

    // Status = CFE_SRL_GetTermiosAttr(Handle, &Termios);
    // if (Status != CFE_SUCCESS) return Status;

    // /**
    //  * Baud Rate Setting
    //  */
    // Status = cfsetispeed(&Termios, Baud);
    // if (Status < 0) return -1; // Revise to `SET_ISPEED_ERR`

    // Status = cfsetospeed(&Termios, Baud);
    // if (Status < 0) return -1; // Revise to `SET_OSPEED_ERR`

    // /**
    //  * Flag setting
    //  */

    // // Control Flag
    // Termios.c_cflag |= (CLOCAL | CREAD); // Local connection, Enable Read
    // Termios.c_cflag &= ~(PARENB | PARODD | CSTOPB); // Parity bit off, 1 Stop bit
    // Termios.c_cflag &= ~CSIZE; // Clear Data bit num
    // Termios.c_cflag |= CS8; // 8 data bits

    // // Local Flag
    // Termios.c_lflag &= ~(ISIG | ICANON); // Neglect Terminal signal (like SIGINT), Read by character
    // Termios.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL); // Echo off
    
    // // Input Flag
    // Termios.c_iflag &= ~(INPCK | ISTRIP); // Parity check off, Masking(8 bit to 7 bit by Mask 0x7F) off
    // Termios.c_iflag &= ~(IXON | IXOFF | IXANY); // Flow Control off

    // // Output Flag
    // Termios.c_oflag &= ~(OPOST); // Post Process off

    // Status = CFE_SRL_SetTermiosAttr(Handle, &Termios);
    // if (Status != CFE_SUCCESS) return Status;

    return CFE_SUCCESS;
}

/**
 * Operational UART setting function
 * Use struct `termios2`
 */
int CFE_SRL_BasicSetUART2(CFE_SRL_IO_Handle_t *Handle, uint32_t BaudRate) {
    int Status;
    struct termios2 Termios2;

    if (Handle == NULL) return CFE_SRL_BAD_ARGUMENT;

    Status = CFE_SRL_BasicIOCTL(Handle->FD, TCGETS2, &Termios2);
    if (Status == -1) {
        Handle->__errno = errno;
        return -1; // Revise to `TCGETS2_ERR`
    }
    /**
     * Baud Rate Setting
     */
    Termios2.c_cflag &= ~CBAUD;
    Termios2.c_cflag |= BOTHER;
    Termios2.c_ispeed = BaudRate;
    Termios2.c_ospeed = BaudRate;

    /**
     * Flag setting
     */
    // Control Flag
    Termios2.c_cflag |= (CLOCAL | CREAD); // Local connection, Enable Read
    Termios2.c_cflag &= ~(PARENB | PARODD | CSTOPB); // Parity bit off, 1 Stop bit
    Termios2.c_cflag &= ~CSIZE; // Clear Data bit num
    Termios2.c_cflag |= CS8; // 8 data bits

    // Local Flag
    Termios2.c_lflag &= ~(ISIG | ICANON); // Neglect Terminal signal (like SIGINT), Read by character
    Termios2.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL); // Echo off
    
    // Input Flag
    Termios2.c_iflag &= ~(INPCK | ISTRIP); // Parity check off, Masking(8 bit to 7 bit by Mask 0x7F) off
    Termios2.c_iflag &= ~(IXON | IXOFF | IXANY); // Flow Control off

    // Output Flag
    Termios2.c_oflag &= ~(OPOST); // Post Process off
<<<<<<< HEAD

=======

    Status = CFE_SRL_BasicIOCTL(Handle->FD, TCSETS2, &Termios2);
    if (Status == -1) {
        Handle->__errno = errno;
        return -1; // Revise to `TCSETS2_ERR`
    }
    return CFE_SUCCESS;
}

/**
 * Change UART Baud rate
 */
int CFE_SRL_ChangeBaudUART(CFE_SRL_IO_Handle_t *Handle, uint32_t BaudRate) {
    int Status;
    struct termios2 Termios2;

    if (Handle == NULL) return CFE_SRL_BAD_ARGUMENT;

    Status = CFE_SRL_BasicIOCTL(Handle->FD, TCGETS2, &Termios2);
    if (Status == -1) {
        Handle->__errno = errno;
        return -1; // Revise to `TCGETS2_ERR`
    }
    /**
     * Baud Rate Setting
     */
    Termios2.c_cflag &= ~CBAUD;
    Termios2.c_cflag |= BOTHER;
    Termios2.c_ispeed = BaudRate;
    Termios2.c_ospeed = BaudRate;

>>>>>>> 9da93d5463f7574bd99ace62fa6f91688270af48
    Status = CFE_SRL_BasicIOCTL(Handle->FD, TCSETS2, &Termios2);
    if (Status == -1) {
        Handle->__errno = errno;
        return -1; // Revise to `TCSETS2_ERR`
    }
    return CFE_SUCCESS;
}

/**
 * Change UART Baud rate
 */
int CFE_SRL_ChangeBaudUART(CFE_SRL_IO_Handle_t *Handle, uint32_t BaudRate) {
    int Status;
    struct termios2 Termios2;

    if (Handle == NULL) return CFE_SRL_BAD_ARGUMENT;

    Status = CFE_SRL_BasicIOCTL(Handle->FD, TCGETS2, &Termios2);
    if (Status == -1) {
        Handle->__errno = errno;
        return -1; // Revise to `TCGETS2_ERR`
    }
    /**
     * Baud Rate Setting
     */
    Termios2.c_cflag &= ~CBAUD;
    Termios2.c_cflag |= BOTHER;
    Termios2.c_ispeed = BaudRate;
    Termios2.c_ospeed = BaudRate;

    Status = CFE_SRL_BasicIOCTL(Handle->FD, TCSETS2, &Termios2);
    if (Status == -1) {
        Handle->__errno = errno;
        return -1; // Revise to `TCSETS2_ERR`
    }
    return CFE_SUCCESS;
}

/*************************************************************
 * 
 *  Basic GPIO Function
 * 
 *************************************************************/
int32 CFE_SRL_BasicGpioOpen(CFE_SRL_GPIO_Handle_t *Handle, const char *Path) {
    if (Handle == NULL || Path == NULL) return CFE_SRL_BAD_ARGUMENT;

    Handle->Chip = gpiod_chip_open(Path);
    if (Handle->Chip == NULL) return -1; // Revise to `GPIO_OPEN_ERR`

    

    return CFE_SUCCESS;
}

int32 CFE_SRL_BasicGpioGetLine(CFE_SRL_GPIO_Handle_t *Handle, unsigned int Line) {
    if (Handle == NULL) return CFE_SRL_BAD_ARGUMENT;

    Handle->Line = gpiod_chip_get_line(Handle->Chip, Line);
    if (Handle->Line == NULL) return -1; // Revise to `GPIO_GET_LINE_ERR`

    return CFE_SUCCESS;
}

int32 CFE_SRL_BasicGpioSetOutput(CFE_SRL_GPIO_Handle_t *Handle, const char *Name, bool Default) {
    int32 Status;

    if (Handle == NULL) return CFE_SRL_BAD_ARGUMENT;

    Status = gpiod_line_request_output(Handle->Line, Name, Default);
    if (Status < 0) return -1; // Revise to `GPIO_SET_OUTPUT_ERR`

    return CFE_SUCCESS;
}

int32 CFE_SRL_BasicGpioSetValue(CFE_SRL_GPIO_Handle_t *Handle, bool Value) {
    int32 Status;

    if (Handle == NULL) return CFE_SRL_BAD_ARGUMENT;

    Status = gpiod_line_set_value(Handle->Line, Value ? 1 : 0);
    if (Status < 0) return -1; // Revise to `GPIO_SET_VALUE_ERR`

    return CFE_SUCCESS;
}

int32 CFE_SRL_BasicGpioClose(CFE_SRL_GPIO_Handle_t *Handle) {

    gpiod_line_release(Handle->Line);
    gpiod_chip_close(Handle->Chip);

    return CFE_SUCCESS;
}