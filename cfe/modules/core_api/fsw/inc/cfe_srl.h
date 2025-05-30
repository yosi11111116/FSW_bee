/******************************************************************************
** File: cfe_srl.h
**
** Purpose:
**      This file contains the definitions of cFE Serial communication
**      Application Programmer's Interface
**
**
** Author:   HyeokJin Kweon
**
** P.S.: Source code of functions is located at
**       `cfe/modules/srl/fsw/src/cfe_srl_api.c`
******************************************************************************/
#ifndef CFE_SRL_H
#define CFE_SRL_H

#include "cfe_srl_api_typedefs.h"
#include "cfe_srl_mission_cfg.h"

/// @brief Get IO Handle pointer. **Use this handle pointer to other API function**
/// @param Index Index of Handle table (Refer enum `CFE_SRL_Handle_Indexer_t`)
/// @return Pointer of `CFE_SRL_IO_Handle_t` object
CFE_SRL_IO_Handle_t *CFE_SRL_ApiGetHandle(CFE_SRL_Handle_Indexer_t Index);

/// @brief Write data to external device via various serial comm. protocol
/// @param Handle A Pointer of SRL Handle. Distinguish character device file
/// @param Data  Data want to transmit to external device
/// @param Size Size of the `Data`
/// @param Addr Address of external device. (I2C, CAN) If not needded, put `NULL`
/// @return Only `CFE_SRL_OK`(which is `0`) is success.
int32 CFE_SRL_ApiWrite(CFE_SRL_IO_Handle_t *Handle, const void *Data, size_t Size, uint32_t Addr);

/// @brief Read data from external device via various serial comm. protocol
/// @param Handle A Pointer of SRL Handle. Distinguish character device file
/// @param TxData Data want to transmit to external device. (It might be the specific register of device)
/// @param TxSize Size of `TxData`
/// @param RxData Data want to receive from external device. (It might be the specific value / result of write)
/// @param RxSize Size of `RxData`
/// @param Timeout Blocking time for Read
/// @param Addr Address for external device. (I2C, CAN) If not needed, put `NULL`
/// @return Only `CFE_SRL_OK`(which is `0`) is success.
int32 CFE_SRL_ApiRead(CFE_SRL_IO_Handle_t *Handle, const void *TxData, size_t TxSize, void *RxData, size_t RxSize, uint32_t Timeout, uint32_t Addr);

/// @brief Close Serial Interface
/// @param Handle A Pointer of SRL Handle. Distinguish character device file
/// @return Only `CFE_SRL_OK`(which is `0`) is success.
int32 CFE_SRL_ApiClose(CFE_SRL_IO_Handle_t * Handle);

/// @brief Set specified GPIO PIN to HIGH or LOW
/// @param Handle `CFE_SRL_GPIO_Handle_t` pointer
/// @param Value `true` for HIGH, `false` for LOW
/// @return Only `CFE_SRL_OK`(which is `0`) is success.
int32 CFE_SRL_ApiGpioSet(CFE_SRL_GPIO_Handle_t *Handle, bool Value);

/// @brief CSP Transaction API function via CSP CAN
/// @param Node Destination Node addr
/// @param Port Corresponed port number to specific request
/// @param TxData Request
/// @param TxSize Request size
/// @param RxData Reply
/// @param RxSize Reply size (`0` for no reply. `-1` for unknown size)
/// @return `1` or `reply size` on success, `0` on failure. (error, unmatched length, timeout)
int32 CFE_SRL_ApiTransactionCSP(uint8_t Node, uint8_t Port, void *TxData, int TxSize, void *RxData, int RxSize);


/// @brief Get particular type of parameter of CSP device.
/// Example: **UTRX baud rate get**
/// @code
/// CFE_SRL_ApiGetRparamCSP(GS_PARAM_UINT32, CSP_NODE_UTRX, 1, 0x0004, Value);
/// @endcode
/// @param Type Parameter type. Look up header `gs/param/rparam.h`
/// @param Node CSP device node
/// @param TableId CSP device table ID
/// @param Addr Parameter address in table
/// @param Param Pointer of buffer
/// @return Only `CFE_SRL_OK`(which is `0`) is success
int32 CFE_SRL_ApiGetRparamCSP(uint8_t Type, uint8_t Node, uint8_t TableId, uint16_t Addr, void *Param);

/// @brief Set particluar type of parameter of CSP device
/// @param Type Parameter type. Look up header `gs/param/rparam.h`
/// @param Node CSP device node
/// @param TableId CSP device table ID
/// @param Addr Parameter address in table
/// @param Param Pointer of buffer
/// @return Only `CFE_SRL_OK`(which is `0`) is success
int32 CFE_SRL_ApiSetRparamCSP(uint8_t Type, uint8_t Node, uint8_t TableId, uint16_t Addr, void *Param);
#endif /* CFE_SRL_H */