#include "cfe_srl_io.h"



#include <errno.h>
extern CFE_SRL_Global_Handle_t GlobalHandle[CFE_SRL_GLOBAL_HANDLE_NUM];

int CFE_SRL_Open(CFE_SRL_IO_Handle_t *Handle, const char *DevName, int Option) {
    int Status;

    if (Handle == NULL) {
        return CFE_SRL_NULL_ERR; // Revise to `NULL_ERROR`
    }
    Status = CFE_SRL_BasicOpen(DevName, Option);
    if (Status < 0) {
        Handle->__errno = errno;
        return Status; // Revise to `OPEN_ERR`
    }

    Handle->FD = Status;

    Status = CFE_SRL_SetHandleStatus(Handle, CFE_SRL_HANDLE_STATUS_FD_INIT, true);
    if (Status != CFE_SRL_OK) return Status;

    return CFE_SRL_OK; // Revise to `OK`
}

int CFE_SRL_Write(CFE_SRL_IO_Handle_t *Handle, const void *Data, size_t Size) {
    // int Status;
    ssize_t WriteBytes;

    if (Handle == NULL || Data == NULL) return CFE_SRL_NULL_ERR;

    if(!CFE_SRL_QueryStatus((const CFE_SRL_Global_Handle_t *)Handle, CFE_SRL_HANDLE_STATUS_FD_INIT)) {
        return CFE_SRL_NOT_OPEN_ERR;
    }

    WriteBytes = CFE_SRL_BasicWrite(Handle->FD, Data, Size);
    if (WriteBytes < 0) {
        Handle->TxErrCnt++;
        Handle->__errno = errno;
        return CFE_SRL_WRITE_ERR;
    }

    Handle->TxCount += WriteBytes;

    if (WriteBytes != Size) {
        Handle->TxErrCnt++;
        Handle->__errno = errno;
        return CFE_SRL_PARTIAL_WRITE_ERR;
    }

    return CFE_SRL_OK;
}

int CFE_SRL_Read(CFE_SRL_IO_Handle_t *Handle, void *Data, size_t Size, uint32_t Timeout) {
    // int Status;
    ssize_t ReadBytes;

    if (Handle == NULL || Data == NULL) return CFE_SRL_NULL_ERR;
    
    if (!CFE_SRL_QueryStatus((CFE_SRL_Global_Handle_t *)Handle, CFE_SRL_HANDLE_STATUS_FD_INIT)) {
        return CFE_SRL_NOT_OPEN_ERR;        
    }
    
    ReadBytes = CFE_SRL_BasicPollRead(Handle->FD, Data, Size, Timeout);
    
    if (ReadBytes == CFE_SRL_TIMEOUT_ERR) {
        Handle->RxErrCnt ++;
        Handle->__errno = errno;
        return CFE_SRL_TIMEOUT_ERR;
    }
    else if (ReadBytes == CFE_SRL_ERROR) {
        Handle->RxErrCnt ++;
        Handle->__errno = errno;
        return CFE_SRL_READ_ERR;
    }

    Handle->RxCount += ReadBytes;

    if (ReadBytes != Size) {
        Handle->RxErrCnt ++;
        Handle->__errno = errno;
        return CFE_SRL_PARTIAL_READ_ERR;
    }
    
    return CFE_SRL_OK;
}

int CFE_SRL_TransactionI2C(CFE_SRL_IO_Handle_t *Handle, const void *TxData, size_t TxSize, void *RxData, size_t RxSize, uint32_t Addr) {
    int Status;
    struct i2c_rdwr_ioctl_data Packet;
    struct i2c_msg MsgI2C[2];

    // First Message - Write
    MsgI2C[0].addr = (uint16_t)Addr;
    MsgI2C[0].flags = 0;    // Write flag
    MsgI2C[0].len = TxSize;
    MsgI2C[0].buf = (uint8_t *)TxData;

    // Second Message - Read
    MsgI2C[1].addr = (uint16_t)Addr;
    MsgI2C[1].flags = I2C_M_RD; // Read flag
    MsgI2C[1].len = RxSize;
    MsgI2C[1].buf = RxData;

    /**
     * Configure Transaction Packet
     */
    Packet.nmsgs = 2;
    Packet.msgs = MsgI2C;

    /**
     * Do transaction
     */
    Status = CFE_SRL_BasicIOCTL(Handle->FD, I2C_RDWR, &Packet);
    if (Status != CFE_SRL_OK) {
        Handle->__errno = errno;
        return CFE_SRL_READ_ERR;
    }
    
    return CFE_SRL_OK;
}

int CFE_SRL_Close(CFE_SRL_IO_Handle_t *Handle) {
    int Status;

    if (Handle == NULL) return CFE_SRL_NULL_ERR;

    if (!CFE_SRL_QueryStatus((CFE_SRL_Global_Handle_t *)Handle, CFE_SRL_HANDLE_STATUS_FD_INIT)) {
        return CFE_SRL_NOT_OPEN_ERR;
    }
    
    Status = CFE_SRL_BasicClose(Handle->FD);
    if (Status < 0) {
        Handle->__errno = errno;    
        return CFE_SRL_CLOSE_ERR;
    }
    return CFE_SRL_OK;
}


// For CAN
int CFE_SRL_OpenSocket(CFE_SRL_IO_Handle_t *Handle, const char *DevName) {
    int Status;
    int Socket; // Equivalent to File Descriptor
    struct ifreq Ifr;
    struct sockaddr_can Addr = {0};

    Socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (Socket < 0) {
        Handle->__errno = errno;
        return -1; // Revise to `CAN_OPEN_SOCK_ERR`
    }

    strncpy(Ifr.ifr_name, DevName, IFNAMSIZ-1);
    Status = CFE_SRL_BasicIOCTL(Socket, SIOCGIFINDEX, &Ifr); // Get interface index number
    if (Status < 0) {
        Handle->__errno = errno;
        CFE_SRL_BasicClose(Socket);
        return -1; // Revise to `CAN_SIOCGIFINDEX_ERR`
    }

    Addr.can_family = AF_CAN;
    Addr.can_ifindex = Ifr.ifr_ifindex;

    Status = bind(Socket, (struct sockaddr *)&Addr, sizeof(Addr));
    if (Status < 0) {
        Handle->__errno = errno;
        CFE_SRL_BasicClose(Socket);
        return -1; // Revise to `CAN_BIND_ERR`
    }

    Handle->FD = Socket;
    
    Status = CFE_SRL_SetHandleStatus(Handle, CFE_SRL_HANDLE_STATUS_FD_INIT, true);
    if (Status != CFE_SRL_OK) return Status;

    return CFE_SRL_OK;
}

/*************************************************************
 * 
 *  IO GPIO Function
 * 
 *************************************************************/
int CFE_SRL_GpioInit(CFE_SRL_GPIO_Handle_t *Handle, const char *Path, unsigned int Line, const char *Name, bool Default) {
    int Status;

    if (Handle == NULL || Path == NULL) return CFE_SRL_NULL_ERR;

    Status = CFE_SRL_BasicGpioOpen(Handle, Path);
    if (Status != CFE_SRL_OK) return Status;

    Status = CFE_SRL_BasicGpioGetLine(Handle, Line);
    if (Status != CFE_SRL_OK) return Status;

    Status = CFE_SRL_BasicGpioSetOutput(Handle, Name, Default);
    if (Status != CFE_SRL_OK) return Status;

    return CFE_SRL_OK;
}
