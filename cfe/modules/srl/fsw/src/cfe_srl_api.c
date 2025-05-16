/******************************************************************************
** File: cfe_srl_api.c
**
** Purpose:
**      This file contains the source code for the SRL API's.
**
** Notes: The following 4 terms have been, or are used in the cFS architecture and implementation
**
**      Handle -
**
**      GlobalHandle  -
**
**      MutexID - 
**
**
** Author:   HyeokJin Kweon
**
** P.S.: Declarations of function are located at
**       `cfe/modules/core_api/fsw/inc/cfe_srl.h`
******************************************************************************/

/*
** Include Files
*/
#include "cfe_srl_module_all.h"


/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SRL_ApiWrite(CFE_SRL_IO_Handle_t *Handle, const void *Data, size_t Size, uint32_t Addr) {
    int32 Status;
    CFE_SRL_DevType_t DevType;

    if (Handle == NULL || Data == NULL) return CFE_SRL_NULL_ERR;

    DevType = CFE_SRL_GetHandleDevType(Handle);

    switch(DevType) {
        case SRL_DEVTYPE_I2C:
            Status = CFE_SRL_WriteI2C(Handle, Data, Size, (uint8_t)Addr);
            break;
        case SRL_DEVTYPE_UART:
        case SRL_DEVTYPE_RS422:
            Status = CFE_SRL_WriteUART(Handle, Data, Size);
            break;
        case SRL_DEVTYPE_CAN:
            Status = CFE_SRL_WriteCAN(Handle, Data, Size, Addr);
            break;
        default:
            Status = -1; // Revise to `INVALID_DEV_TYPE_ERR`
            break;
    }

    return Status;
}


/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SRL_ApiRead(CFE_SRL_IO_Handle_t *Handle, const void *TxData, size_t TxSize, void *RxData, size_t RxSize, uint32_t Timeout, uint32_t Addr) {
    int32 Status;
    CFE_SRL_DevType_t DevType;

    if (Handle == NULL || TxData == NULL || RxData == NULL) return CFE_SRL_NULL_ERR;

    DevType = CFE_SRL_GetHandleDevType(Handle);

    switch(DevType) {
        case SRL_DEVTYPE_I2C:
            Status = CFE_SRL_ReadI2C(Handle, TxData, TxSize, RxData, RxSize, Addr);
            break;
        case SRL_DEVTYPE_UART:
        case SRL_DEVTYPE_RS422:
            Status = CFE_SRL_ReadUART(Handle, TxData, TxSize, RxData, RxSize, Timeout);
            break;
        case SRL_DEVTYPE_CAN:
            Status = CFE_SRL_ReadCAN(Handle, TxData, TxSize, RxData, RxSize, Timeout, Addr);
            break;
        default:
            Status = -1; // Revise to `INVALID_DEV_TYPE_ERR`
            break;
    }
    return Status;
}


/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SRL_ApiClose(CFE_SRL_IO_Handle_t *Handle) {
    int32 Status;

    if (Handle == NULL) return CFE_SRL_NULL_ERR;

    Status = CFE_SRL_HandleClose(Handle);
    if (Status != CFE_SRL_OK) return Status;

    return CFE_SRL_OK;
}


/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SRL_ApiGpioSet(CFE_SRL_GPIO_Handle_t *Handle, bool Value) {
    if (Handle == NULL) return CFE_SRL_NULL_ERR;
    
    return CFE_SRL_BasicGpioSetValue(Handle, Value);
}