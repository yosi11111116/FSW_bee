#ifndef CFE_SRL_MUTEX_H
#define CFE_SRL_MUTEX_H

#include <pthread.h>
#include <stdbool.h>

#include "cfe_srl_basic.h"

typedef struct {
    pthread_mutex_t Mutex;
    bool Isinit;
} CFE_SRL_IO_Handle_Mutex_t;


extern pthread_mutex_t GlobalHandleMutex;
// Index : `MutexID`
extern CFE_SRL_IO_Handle_Mutex_t IOMutex[CFE_SRL_GNRL_DEVICE_NUM];


int CFE_SRL_GlobalHandleMutexInit(void);
int CFE_SRL_GlobalHandleMutexLock(void);
int CFE_SRL_GlobalHandleMutexUnlock(void);

int CFE_SRL_SetHandleMutexID(CFE_SRL_IO_Handle_t *Handle, uint8_t MutexID);
int CFE_SRL_HandleMutexInit(CFE_SRL_IO_Handle_t *Handle, uint8_t MutexID);

int CFE_SRL_MutexLock(CFE_SRL_IO_Handle_t *Handle);
int CFE_SRL_MutexUnlock(CFE_SRL_IO_Handle_t *Handle);
int CFE_SRL_MutexDestroy(CFE_SRL_IO_Handle_t *Handle);

#endif /* CFE_SRL_MUTEX_H */