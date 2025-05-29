#ifndef CFE_SRL_HANDLE_H
#define CFE_SRL_HANDLE_H


#define CFE_SRL_STUB
#undef CFE_SRL_STUB

#define CFE_SRL_UT

#include <stdint.h>

#ifdef CFE_SRL_STUB
#include "cfe_srl_io_stub.h"
#else
#include "cfe_srl_io.h"
#endif

#include "cfe_srl_mutex.h"

/**
 * Declaration of Struct & Values are located in `cfe_srl_basic.h`
*/


int CFE_SRL_DeviceTableInit(void);
int CFE_SRL_PriorInit(void);
int CFE_SRL_AddSubRefCount(const char *DevName, bool Add);
int CFE_SRL_GetOpenOption(CFE_SRL_DevType_t Devtype);
int CFE_SRL_DevOpenCheck(const char *DevName);
int CFE_SRL_DevCloseCheck(const char *DevName);
int CFE_SRL_DevOpenInsert(const char *DevName, int FD);
int CFE_SRL_DevCloseRemove(const char *DevName);
int CFE_SRL_GetHandleByName(const char *Name, CFE_SRL_IO_Handle_t **Handle);
int CFE_SRL_GlobalHandleInit(CFE_SRL_IO_Handle_t **Handle, const char *Name, const char *DevName, uint8_t DevType);
int CFE_SRL_HandleInit(CFE_SRL_IO_Handle_t **Handle, const char *Name, const char *Devname, uint8_t DevType, uint8_t MutexID);
int CFE_SRL_HandleClose(CFE_SRL_IO_Handle_t *Handle);

#endif /* CFE_SRL_HANDLE_H */