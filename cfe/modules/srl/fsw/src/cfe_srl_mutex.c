#include "cfe_srl_mutex.h"

pthread_mutex_t GlobalHandleMutex;

// Index : `MutexID`
CFE_SRL_IO_Handle_Mutex_t IOMutex[CFE_SRL_GNRL_DEVICE_NUM];

int CFE_SRL_GlobalHandleMutexInit(void) {
    int Status;
    pthread_mutexattr_t Attr;
    
    Status = pthread_mutexattr_init(&Attr);
    if (Status != 0) return CFE_SRL_MUTEX_ATTR_INIT_ERR;

#if defined __USE_POSIX199506 || defined __USE_UNIX98
    Status = pthread_mutexattr_setprotocol(&Attr, PTHREAD_PRIO_INHERIT);
    if (Status != 0) return CFE_SRL_MUTEX_SET_PROTOCOL_ERR;
#endif

    Status = pthread_mutex_init(&GlobalHandleMutex, &Attr);
    if (Status != 0) return CFE_SRL_MUTEX_INIT_ERR;

    return CFE_SUCCESS; // Revise to `OK`
}

int CFE_SRL_GlobalHandleMutexLock(void) {
    int Status;
    
    Status = pthread_mutex_lock(&GlobalHandleMutex);
    
    return Status < 0 ? CFE_SRL_MUTEX_LOCK_ERR : CFE_SUCCESS;
}

int CFE_SRL_GlobalHandleMutexUnlock(void) {
    int Status;

    Status = pthread_mutex_unlock(&GlobalHandleMutex);

    return Status < 0 ? CFE_SRL_MUTEX_UNLOCK_ERR : CFE_SUCCESS;     // Revise to `UNLOCK_ERR` : `OK`
}

int CFE_SRL_SetHandleMutexID(CFE_SRL_IO_Handle_t *Handle, uint8_t MutexID) {
    if (Handle == NULL) return -1; // Revise to `NULL_ERROR`

    CFE_SRL_Global_Handle_t *Entry;
    Entry = (CFE_SRL_Global_Handle_t *)Handle;

    Entry->MutexID = MutexID;
    
    return CFE_SUCCESS; // Revise to `OK`
}

int CFE_SRL_HandleMutexInit(CFE_SRL_IO_Handle_t *Handle, uint8_t MutexID) {
    int Status;
    pthread_mutexattr_t Attr;

    if (Handle == NULL) return CFE_SRL_BAD_ARGUMENT; // Revise to `NULL_ERROR`

    if (MutexID > CFE_SRL_GNRL_DEVICE_NUM) {
        OS_printf("cFE SRL Handle Mutex Init Error. Mutex ID %u is NOT effective.\n", MutexID);
        return -1; // Revise to `MUTEX_NUM_ERR`
    }

    if (IOMutex[MutexID].Isinit) {
        CFE_SRL_SetHandleMutexID(Handle, MutexID);
        // Set status to `MUTEX_INIT`
    }

    Status = pthread_mutexattr_init(&Attr);
    if (Status != 0) return CFE_SRL_MUTEX_ATTR_INIT_ERR; // Revise to `ATTR_INIT_ERR`

#if defined __USE_POSIX199506 || defined __USE_UNIX98
    Status = pthread_mutexattr_setprotocol(&Attr, PTHREAD_PRIO_INHERIT);
    if (Status != 0) return -1; // Revise to `MUTEX_SET_PRTOCOL_ERROR`
#endif

    Status = pthread_mutex_init(&IOMutex[MutexID].Mutex, &Attr);
    if (Status != 0) return CFE_SRL_MUTEX_INIT_ERR; // Revise to `MUTEX_INIT_ERR`

    IOMutex[MutexID].Isinit = true;
    CFE_SRL_SetHandleMutexID(Handle, MutexID);
    // Set Status to `MUTEX_INIT`

    Status = CFE_SRL_SetHandleStatus(Handle, CFE_SRL_HANDLE_STATUS_MUTEX_INIT, true);
    if (Status != CFE_SUCCESS) return Status;

    return CFE_SUCCESS; // Revise to `OK`
}

int CFE_SRL_MutexLock(CFE_SRL_IO_Handle_t *Handle) {
    int Status;
    
    if (Handle == NULL) return CFE_SRL_BAD_ARGUMENT;

    const CFE_SRL_Global_Handle_t *Entry = (CFE_SRL_Global_Handle_t *)Handle;

    if (IOMutex[Entry->MutexID].Isinit == false) return -1; // Revise to `MUTEX_NOT_INIT_ERR`

    Status = pthread_mutex_lock(&IOMutex[Entry->MutexID].Mutex);
    if (Status < 0) {
        Handle->__errno = errno;
        return CFE_SRL_MUTEX_LOCK_ERR;
    }

    return CFE_SUCCESS;
}

int CFE_SRL_MutexUnlock(CFE_SRL_IO_Handle_t *Handle) {
    int Status;

    if (Handle == NULL) return CFE_SRL_BAD_ARGUMENT;

    const CFE_SRL_Global_Handle_t *Entry = (CFE_SRL_Global_Handle_t *)Handle;

    if (IOMutex[Entry->MutexID].Isinit == false) return -1; // Revise to `MUTEX_NOT_INIT_ERR`

    Status = pthread_mutex_unlock(&IOMutex[Entry->MutexID].Mutex);
    if (Status < 0) {
        Handle ->__errno = errno;
        return CFE_SRL_MUTEX_UNLOCK_ERR;
    }

    return CFE_SUCCESS;
}

int CFE_SRL_MutexDestroy(CFE_SRL_IO_Handle_t *Handle) {
    int Status;

    if (Handle == NULL) return CFE_SRL_BAD_ARGUMENT;

    const CFE_SRL_Global_Handle_t *Entry = (CFE_SRL_Global_Handle_t *)Handle;

    if (IOMutex[Entry->MutexID].Isinit == false) return CFE_SUCCESS;
    else {
        Status = pthread_mutex_destroy(&IOMutex[Entry->MutexID].Mutex);
        if (Status < 0) {
            Handle->__errno = errno;
            return CFE_SRL_MUTEX_INIT_ERR;
        }
        IOMutex[Entry->MutexID].Isinit = false;
        return CFE_SUCCESS;
    }
}