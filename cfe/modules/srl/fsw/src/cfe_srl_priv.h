#ifndef CFE_SRL_PRIV_H
#define CFE_SRL_PRIV_H

#include "cfe_srl_eventids.h"
#include "cfe_srl_handle.h"

int32 CFE_SRL_WriteI2C(CFE_SRL_IO_Handle_t *Handle, const void *Data, size_t Size, uint8_t Addr);
int32 CFE_SRL_WriteUART(CFE_SRL_IO_Handle_t *Handle, const void *Data, size_t Size);
int32 CFE_SRL_WriteCAN(CFE_SRL_IO_Handle_t *Handle, const void *Data, size_t Size, uint32_t Addr);

/// @brief Private I2C Read function via `ioctl` transaction
/// @param Handle 
/// @param RxData 
/// @param RxSize 
/// @param TxData 
/// @param TxSize 
/// @param Addr Slave Address
/// @return 
int32 CFE_SRL_ReadI2C(CFE_SRL_IO_Handle_t *Handle, const void *TxData, size_t TxSize, void *RxData, size_t RxSize, uint32_t Addr);

/// @brief Private UART Read function by Write -> Poll Read
/// @param Handle 
/// @param TxData 
/// @param TxSize 
/// @param RxData 
/// @param RxSize 
/// @param Timeout 
/// @return 
int32 CFE_SRL_ReadUART(CFE_SRL_IO_Handle_t *Handle, const void *TxData, size_t TxSize, void *RxData, size_t RxSize, uint32_t Timeout);

/// @brief Private CAN Read function by Write -> Poll Read
/// @param Handle 
/// @param TxData 
/// @param TxSize 
/// @param RxData 
/// @param RxSize 
/// @param Timeout 
/// @param Addr External Device Address
/// @return 
int32 CFE_SRL_ReadCAN(CFE_SRL_IO_Handle_t *Handle, const void *TxData, size_t TxSize, void *RxData, size_t RxSize, uint32_t Timeout, uint32_t Addr);

#endif /* CFE_SRL_PRIV_H */