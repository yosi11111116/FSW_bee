#include "cfe_srl_handle.h"

#include <pthread.h>
#include <string.h>

#include <fcntl.h>

CFE_SRL_Global_Handle_t GlobalHandle[CFE_SRL_GLOBAL_HANDLE_NUM] = {0};
CFE_SRL_Open_Device_Handle_t DeviceTable[CFE_SRL_GLOBAL_HANDLE_NUM] = {0};
// pthread_mutex_t GlobalHandleMutex;

#ifndef CFE_SRL_UT
char *DeviceList[CFE_SRL_GNRL_DEVICE_NUM] = {(char *)"/dev/i2c-0\0", (char *)"/dev/i2c-1\0", (char *)"/dev/i2c-2\0", (char *)"/dev/ttyS0\0", (char *)"/dev/ttyS1\0", (char *)"can0\0"};
#else
char *DeviceList[CFE_SRL_GNRL_DEVICE_NUM] = {(char *)"/dev/pts/3\0", (char *)"/dev/i2c-1\0", (char *)"/dev/i2c-2\0", (char *)"/dev/ttyS0\0", (char *)"/dev/ttyS1\0", (char *)"can0\0"};
#endif
int CFE_SRL_DeviceTableInit(void) {
    memset(DeviceTable, 0, sizeof(DeviceTable));

    for (uint8_t i = 0; i < CFE_SRL_GNRL_DEVICE_NUM; i++) {
        strcpy(DeviceTable[i].DevName, DeviceList[i]);
    }
    CFE_ES_WriteToSysLog("%s: DeviceTable Initialized. 1: %s 2: %s 3: %s 4: %s 5: %s 6: %s.",
        __func__, DeviceTable[0].DevName, DeviceTable[1].DevName, DeviceTable[2].DevName, DeviceTable[3].DevName, DeviceTable[4].DevName, DeviceTable[5].DevName);
    return CFE_SRL_OK;
}

int CFE_SRL_PriorInit(void) {
    int Status;
    CFE_SRL_DeviceTableInit();
    Status = CFE_SRL_GlobalHandleMutexInit();
    if (Status != CFE_SRL_OK) {
        return CFE_SRL_PRIOR_INIT_ERR; // Revise to `PRIOR_INIT_ERROR`
    }

    return CFE_SRL_OK; // Revise to `OK`
}

int CFE_SRL_AddSubRefCount(const char *DevName, bool Add) {
    for (uint8_t i = 0; i < CFE_SRL_GLOBAL_HANDLE_NUM; i++) {
        if (strcmp(DeviceTable[i].DevName, DevName) == 0) {
            if (Add) { // `true` for add
                DeviceTable[i].RefCount++;
                return CFE_SRL_OK;
            }
            else { // `false` for sub
                DeviceTable[i].RefCount--;
                return CFE_SRL_OK;
            }
        }
    }
    return CFE_SRL_NOT_FOUND; // Revise to `NOT_FOUND`
}

int CFE_SRL_GetOpenOption(CFE_SRL_DevType_t Devtype) {
    switch (Devtype) {
        case SRL_DEVTYPE_I2C:
        case SRL_DEVTYPE_SPI:
            return O_RDWR;
        case SRL_DEVTYPE_UART:
        case SRL_DEVTYPE_RS422:
            return O_RDWR | O_NOCTTY;
            // return O_RDWR | O_NOCTTY | O_NONBLOCK;
        default:
            break;
    }
    return -1;
}

int CFE_SRL_DevOpenCheck(const char *DevName) {
    for (uint8_t i = 0; i < CFE_SRL_DEVICE_TABLE_NUM; i++) {
        if (strcmp(DeviceTable[i].DevName, DevName) == 0) {
            if (DeviceTable[i].FD > 0) {
                return DeviceTable[i].FD;
            }
            else return CFE_SRL_NOT_OPENED; // revise to `NOT_OPENED`
        }
    }
    return CFE_SRL_NOT_FOUND; // revise to `NOT_FOUND_ERROR`
}

int CFE_SRL_DevCloseCheck(const char *DevName) {
    for (uint8_t i = 0; i < CFE_SRL_DEVICE_TABLE_NUM; i++) {
        if (strcmp(DeviceTable[i].DevName, DevName) == 0) {
            if (DeviceTable[i].RefCount == 1) {
                return CFE_SRL_OK;
            }
            else return CFE_SRL_REF_REMAIN;
        }
    }
    return CFE_SRL_NOT_FOUND;
}

int CFE_SRL_DevOpenInsert(const char *DevName, int FD) {
    for (uint8_t i = 0; i < CFE_SRL_DEVICE_TABLE_NUM; i++) {
        if (strcmp(DeviceTable[i].DevName, DevName) == 0) {
            DeviceTable[i].FD = FD;
            DeviceTable[i].RefCount ++;
            CFE_ES_WriteToSysLog("%s: Device %s is opened in FD %d, RefCnt %d.", __func__, DeviceTable[i].DevName, DeviceTable[i].FD, DeviceTable[i].RefCount);
            return CFE_SRL_OK; // Revise to `OK`
        }
    }
    return CFE_SRL_DEV_NOT_FOUND; // Revise to `DEV_NOT_FOUND`
}

int CFE_SRL_DevCloseRemove(const char *DevName) {
    for (uint8 i = 0; i < CFE_SRL_DEVICE_TABLE_NUM; i++) {
        if (strcmp(DeviceTable[i].DevName, DevName) == 0) {
            DeviceTable[i].FD = -1;
            DeviceTable[i].RefCount = 0;
            CFE_ES_WriteToSysLog("%s: Device %s is closed.", __func__, DevName);
            return CFE_SRL_OK;
        }
    }
    return CFE_SRL_DEV_NOT_FOUND;
}

int CFE_SRL_GetHandleByName(const char *Name, CFE_SRL_IO_Handle_t **Handle) {
    CFE_SRL_Global_Handle_t *Entry;

    if (!Name) {
        return CFE_SRL_NAME_PTR_NULL_ERR; // revise to `NAME_PTR_NULL_ERROR`
    }
    size_t NameLength = strlen(Name);
    if (NameLength > CFE_SRL_HANDLE_NAME_LENGTH) {
        return CFE_SRL_NAME_LEN_OVERFLOW_ERR; // revise to `NAME_LEN_OVERFLOW`
    }

    //Mutex Lock
    CFE_SRL_GlobalHandleMutexLock();

    // Init
    for (uint8_t i = 0; i < CFE_SRL_GLOBAL_HANDLE_NUM; i++) {
        Entry = &GlobalHandle[i];
        if (CFE_SRL_QueryStatus(Entry, CFE_SRL_HANDLE_STATUS_ALLOCATE) && strcmp(Entry->Name, Name) == 0) { // Name matched
            *Handle = &Entry->Handle;
            // Mutex Unlock
            CFE_SRL_GlobalHandleMutexUnlock();
            return CFE_SRL_OK; // Revise to `OK` == `Found`
        }
    }
    // Mutex Unlock
    CFE_SRL_GlobalHandleMutexUnlock();

    return CFE_SRL_NOT_FOUND; //Revise to `NOT_FOUND`
}

int CFE_SRL_GlobalHandleInit(CFE_SRL_IO_Handle_t **Handle, const char *Name, const char *DevName, uint8_t DevType) {
    CFE_SRL_Global_Handle_t *Entry;
    CFE_SRL_IO_Handle_t *TempHandle;
    int Status;

    Status = CFE_SRL_GetHandleByName(Name, &TempHandle);
    if (Status == CFE_SRL_NAME_PTR_NULL_ERR || 
        Status == CFE_SRL_NAME_LEN_OVERFLOW_ERR) return Status;

    if (Status == CFE_SRL_OK) { // Revise to `OK`
        return CFE_SRL_ALREADY_EXIST; // Revise to `ALREADY_EXIST`
    }
    
    // Mutex Lock
    CFE_SRL_GlobalHandleMutexLock();

    // If Not Found, Do Main process
    for (uint8_t i = 0; i < CFE_SRL_GLOBAL_HANDLE_NUM; i++) {
        Entry = &GlobalHandle[i];
        if (!CFE_SRL_QueryStatus(Entry, CFE_SRL_HANDLE_STATUS_ALLOCATE)) {
            memset(Entry, 0, sizeof(CFE_SRL_Global_Handle_t));
            
            strcpy(Entry->Name, Name);
            Entry->Name[sizeof(Entry->Name)-1] = '\0';
            
            Entry->DevType = DevType;
            
            strcpy(Entry->DevName, DevName);
            Entry->DevName[sizeof(Entry->DevName)-1] = '\0';
            
            Entry->Status = CFE_SRL_HANDLE_STATUS_ALLOCATE;
            *Handle = &Entry->Handle;

            //Mutex Unlock
            CFE_SRL_GlobalHandleMutexUnlock();
            return CFE_SRL_OK; // Revise to `OK`
        }
    }
    //Mutex Unlock
    CFE_SRL_GlobalHandleMutexUnlock();
    return CFE_SRL_FULL_ERR; // Revise to `FULL_ERROR`
}



int CFE_SRL_HandleInit(CFE_SRL_IO_Handle_t **Handle, const char *Name, const char *Devname, uint8_t DevType, uint8_t MutexID) {
    int Status;
    CFE_SRL_IO_Handle_t *TempHandle;
    int OpenOption;

    // Mutex value check


    // Get Open Option
    OpenOption = CFE_SRL_GetOpenOption(DevType);

    // GlobalHandle Init
    Status = CFE_SRL_GlobalHandleInit(&TempHandle, Name, Devname, DevType);
    if (Status != CFE_SRL_OK) return Status;     // Revise `1` to `OK`

    // Open
    int FD = CFE_SRL_DevOpenCheck(Devname);
    if (FD > 0) { // If already opened, 
        (*Handle)->FD = FD; // Just substitute the FD value
        CFE_SRL_AddSubRefCount(Devname, true);
    }
    else { // If not opened,
        if (DevType == SRL_DEVTYPE_CAN) {
            Status = CFE_SRL_OpenSocket(TempHandle, Devname); // Do socket()
            if (Status != CFE_SRL_OK) return Status;
        }
        else {
            Status = CFE_SRL_Open(TempHandle, Devname, OpenOption); // Do open()
            if (Status != CFE_SRL_OK) return Status;
        }

        Status = CFE_SRL_DevOpenInsert(Devname, TempHandle->FD); // Add to `DeviceTable`
        if (Status != CFE_SRL_OK) return Status;
    }
    
    // IO Handle Mutex Init
    Status = CFE_SRL_HandleMutexInit(TempHandle, MutexID);
    if (Status != CFE_SRL_OK) return Status;     // Revise `1` to `OK`

    // Allocate the result Handle
    *Handle = TempHandle;
    
    if (DevType == SRL_DEVTYPE_UART || DevType == SRL_DEVTYPE_RS422) {
        Status = CFE_SRL_BasicSetUART(*Handle, 115200);

        if (Status != CFE_SRL_OK) return CFE_SRL_UART_INIT_ERR;
    }
    return CFE_SRL_OK; // Revise to `OK`
}


int CFE_SRL_HandleClose(CFE_SRL_IO_Handle_t *Handle) {
    int Status;
    CFE_SRL_Global_Handle_t *Entry;

    if (Handle == NULL) return CFE_SRL_NULL_ERR;

    Entry = (CFE_SRL_Global_Handle_t *)Handle;

    Status = CFE_SRL_DevCloseCheck(Entry->DevName);
    if (Status == CFE_SRL_OK) {
        Status = CFE_SRL_Close(Handle);
        if (Status != CFE_SRL_OK) return Status;

        Status = CFE_SRL_DevCloseRemove(Entry->DevName);
        if (Status != CFE_SRL_OK) return Status;
        else return CFE_SRL_OK;
    }
    else if (Status == CFE_SRL_REF_REMAIN) {
        Status = CFE_SRL_AddSubRefCount(Entry->DevName, false);
        if (Status != CFE_SRL_OK) return Status;
        return CFE_SRL_OK;
    }
    else return Status;
}