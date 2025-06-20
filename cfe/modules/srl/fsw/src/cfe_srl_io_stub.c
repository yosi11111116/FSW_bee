#include "cfe_srl_io_stub.h"

#include <errno.h>
extern CFE_SRL_Global_Handle_t GlobalHandle[CFE_SRL_GLOBAL_HANDLE_NUM];

int CFE_SRL_Open(CFE_SRL_IO_Handle_t *Handle, const char *DevName, int Option) {

    Handle->FD = 311;
    CFE_SRL_SetHandleStatus(Handle, CFE_SRL_HANDLE_STATUS_FD_INIT, true);
    
    return CFE_SUCCESS; // Revise to `OK`
}

int CFE_SRL_Write(CFE_SRL_IO_Handle_t *Handle, const void *Data, size_t Size) {
    return CFE_SUCCESS;
}

int CFE_SRL_Read(CFE_SRL_IO_Handle_t *Handle, void *Data, size_t Size, uint32_t Timeout) {
    return CFE_SUCCESS;
}

int CFE_SRL_TransactionI2C(CFE_SRL_IO_Handle_t *Handle, const void *TxData, size_t TxSize, void *RxData, size_t RxSize, uint32_t Addr) {
    return CFE_SUCCESS;
}

int CFE_SRL_Close(CFE_SRL_IO_Handle_t *Handle) {
    return CFE_SUCCESS;
}

int CFE_SRL_OpenSocket(CFE_SRL_IO_Handle_t *Handle, const char *DevName) {

    Handle->FD = 361;
    CFE_SRL_SetHandleStatus(Handle, CFE_SRL_HANDLE_STATUS_FD_INIT, true);
    
    return CFE_SUCCESS; // Revise to `OK`
}

int CFE_SRL_GpioInit(CFE_SRL_GPIO_Handle_t *Handle, const char *Path, unsigned int Line, const char *Name, bool Default) {
    
    return CFE_SUCCESS;
}