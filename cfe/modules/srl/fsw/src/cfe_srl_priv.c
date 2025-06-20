/************************************************************************
 * Author : Hyeokjin Kweon
 * 
 * Last Modified : 2025 - 04 - 27
 * 
 * Purpose : Serial Comm. Core Module's API Initialization
 ************************************************************************/

/**
 * Required header files
*/
#include "cfe_srl_module_all.h"

// #define I2C0_READY
// #define CAN_READY
// #define UART_READY
// #define RS422_READY
// #define GPIO_READY
/**
 * Global data
 * 
 * Serial Handle for each srl comm.
 */
CFE_SRL_IO_Handle_t *I2C0;
CFE_SRL_IO_Handle_t *I2C1;
CFE_SRL_IO_Handle_t *I2C2;
CFE_SRL_IO_Handle_t *UART;
CFE_SRL_IO_Handle_t *RS422;
CFE_SRL_IO_Handle_t *CAN0;

/**
 * After develop completed, init handle via this table 
 * During development, this type of init process not compatible to each device unit test
 */
CFE_SRL_IO_Handle_t *HandleTable[CFE_SRL_GNRL_DEVICE_NUM];

const char *GeneralHandleNameTable[CFE_SRL_GNRL_DEVICE_NUM] = {};
const char *DeviceNameTable[CFE_SRL_GNRL_DEVICE_NUM] = {};
const CFE_SRL_DevType_t DeviceTypeTable[CFE_SRL_GNRL_DEVICE_NUM] = {};

/**
 * GPIO Global Data - Must Check Real value and revise this
 */
CFE_SRL_GPIO_Handle_t *GPIO[CFE_SRL_TOT_GPIO_NUM];

const char *GpioNameArr[CFE_SRL_TOT_GPIO_NUM] = {(char *)"ADCS EN", (char *)"ADCS BOOT", (char *)"PAY1-1", (char *)"PAY1-2", (char *)"GRX-PPS"};
const char *GpioPathArr[CFE_SRL_TOT_GPIO_NUM] = {(char *)"/dev/gpiochip0", (char *)"/dev/gpiochip0", (char *)"/dev/gpiochip2", (char *)"/dev/gpiochip2", (char *)"/dev/gpiochip2"};
uint32 GpioLineArr[CFE_SRL_TOT_GPIO_NUM] = {28, 29, 3, 5, 4};


/*----------------------------------------------------------------
 *
 * Implemented per public API
 * Early Initialization function executed at cFE ES
 * Append object to `cfe_es_objtab.c`
 * Declaration is located at
 * `cfe/modules/core_private/fsw/inc/cfe_srl_core_internal.h`
 * 
 *-----------------------------------------------------------------*/
int32 CFE_SRL_EarlyInit(void) {
    int32 Status;

    Status = CFE_SRL_PriorInit();
    if (Status != CFE_SUCCESS) {
        return Status;
    }
    CFE_ES_WriteToSysLog("%s: Prior Initialized.", __func__);
    
/****************************************************
 * Serial Comm. Init
 * Append other remain serial dev later
 ***************************************************/
    /**
     * I2C Init
     */
#ifdef I2C0_READY
    Status = CFE_SRL_HandleInit(&I2C0, "UANT_I2C", "/dev/i2c-0", SRL_DEVTYPE_I2C, CFE_SRL_I2C0_MUTEX_IDX, 0);
    if (Status != CFE_SUCCESS) {
        CFE_ES_WriteToSysLog("%s: I2C0 Initialization failed! RC=%d\n", __func__, Status);
        return CFE_SRL_I2C0_INIT_ERR;
    }
    CFE_ES_WriteToSysLog("%s: I2C0 Initialized. FD : %d | Name : %s | DevName : %s | MutexID : %u | Status : %u |", 
                    __func__, I2C0->FD, ((CFE_SRL_Global_Handle_t *)I2C0)->Name, ((CFE_SRL_Global_Handle_t *)I2C0)->DevName, ((CFE_SRL_Global_Handle_t *)I2C0)->MutexID, ((CFE_SRL_Global_Handle_t *)I2C0)->Status);

#endif

    /**
     * CAN Init
     */
#ifdef CAN_READY
    Status = CFE_SRL_HandleInit(&CAN0, "CubeCAN", "can0", SRL_DEVTYPE_CAN, CFE_SRL_CAN0_MUTEX_IDX, 0);
    if (Status != CFE_SUCCESS) {
        CFE_ES_WriteToSysLog("%s: CAN0 Initialization failed! RC=%d\n", __func__, Status);
        return CFE_SRL_CAN_INIT_ERR;
    }
    CFE_ES_WriteToSysLog("%s: CAN0 Initialized. FD : %d | Name : %s | DevName : %s | MutexID : %u | Status : %u |", 
        __func__, CAN0->FD, ((CFE_SRL_Global_Handle_t *)CAN0)->Name, ((CFE_SRL_Global_Handle_t *)CAN0)->DevName, ((CFE_SRL_Global_Handle_t *)CAN0)->MutexID, ((CFE_SRL_Global_Handle_t *)CAN0)->Status);
#endif
    
    /**
     * RS 422 Init
     */
#ifdef RS422_READY
    Status = CFE_SRL_HandleInit(&RS422, "STX_RS422", "/dev/ttyS0", SRL_DEVTYPE_RS422, CFE_SRL_RS422_MUTEX_IDX, 3000000);
    if (Status != CFE_SUCCESS) {
        CFE_ES_WriteToSysLog("%s: RS422 Initialization failed! RC=%d | %s\n", __func__, Status, strerror(RS422->__errno));
        return CFE_SRL_RS422_INIT_ERR;
    }
    CFE_ES_WriteToSysLog("%s: RS422 Initialized. FD : %d | Name : %s | DevName : %s | MutexID : %u | Status : %u |", 
        __func__, RS422->FD, ((CFE_SRL_Global_Handle_t *)RS422)->Name, ((CFE_SRL_Global_Handle_t *)RS422)->DevName, ((CFE_SRL_Global_Handle_t *)RS422)->MutexID, ((CFE_SRL_Global_Handle_t *)RS422)->Status);
#endif

#ifdef GPIO_READY
    /**
     * GPIO Init
     */
    for (uint8 i = 0; i < CFE_SRL_TOT_GPIO_NUM; i++) {
        Status = CFE_SRL_GpioInit(GPIO[i], GpioPathArr[i], GpioLineArr[i], GpioNameArr[i], 0);
        if (Status != CFE_SUCCESS) {
            CFE_ES_WriteToSysLog("%s: GPIO%d Initialization failed! RC=%d\n", __func__, i, Status);
            return CFE_SRL_GPIO_INIT_ERR;
        }
    }
    CFE_ES_WriteToSysLog("%s: GPIO Initialized.", __func__);
#endif

#ifdef SOCAT_READY
    Status = CFE_SRL_HandleInit(&RS422, "STX 422", SOCAT_DEV, SRL_DEVTYPE_RS422, CFE_SRL_RS422_MUTEX_IDX, 250000);
    if (Status != CFE_SUCCESS) {
        CFE_StatusString_t String;
        CFE_ES_WriteToSysLog("%s: RS422 Initialization failed! RC=%d %s\n", __func__, Status, CFE_ES_StatusToString(Status, &String));
        return CFE_SRL_SOCAT_INIT_ERR;
    }
    CFE_ES_WriteToSysLog("%s: RS422 Initialized. FD : %d | Name : %s | DevName : %s | MutexID : %u | Status : %u |", 
        __func__, RS422->FD, ((CFE_SRL_Global_Handle_t *)RS422)->Name, ((CFE_SRL_Global_Handle_t *)RS422)->DevName, ((CFE_SRL_Global_Handle_t *)RS422)->MutexID, ((CFE_SRL_Global_Handle_t *)RS422)->Status);
#endif

#ifdef CSP_READY
    /**
     * CSP Init
     */
    Status = CFE_SRL_InitCSP();
    if (Status != CFE_SUCCESS) {
        CFE_ES_WriteToSysLog("%s: CSP Initialization failed! RC=%d\n", __func__, Status);
        return Status;
    }
    CFE_ES_WriteToSysLog("%s: CSP Initializaed.", __func__);
#endif

    return CFE_SUCCESS;
}


/**
 * Private Get Handle function
 */
/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_SRL_IO_Handle_t *CFE_SRL_GetHandle(CFE_SRL_Handle_Indexer_t Index) {
    return HandleTable[Index];
}


/**
 * Private Get Handle function
 */
/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_SRL_IO_Handle_t *CFE_SRL_GetHandle(CFE_SRL_Handle_Indexer_t Index) {
    return HandleTable[Index];
}

/**
 * Private Write function
 */

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SRL_WriteI2C(CFE_SRL_IO_Handle_t *Handle, const void *Data, size_t Size, uint8_t Addr) {
    int32 Status;
    CFE_SRL_DevType_t DevType;

    if (Handle == NULL || Data == NULL) return CFE_SRL_BAD_ARGUMENT;
    if (Addr > 128) return CFE_SRL_I2C_ADDR_ERR;

    // Check dev type
    DevType = CFE_SRL_GetHandleDevType(Handle);
    if (DevType != SRL_DEVTYPE_I2C) return CFE_SRL_INVALID_TYPE;

    // Mutex Lock
    Status = CFE_SRL_MutexLock(Handle);
    if (Status != CFE_SUCCESS) return Status;

    // Set Slave Addr
    Status = CFE_SRL_BasicIOCTL(Handle->FD, I2C_SLAVE, &Addr);
    if (Status < 0) {
        Handle->__errno = errno;
        return CFE_SRL_IOCTL_ERR;
    }

    // Write
    Status = CFE_SRL_Write(Handle, Data, Size);
    if (Status != CFE_SUCCESS) return Status;

    // Mutex Unlock
    Status = CFE_SRL_MutexUnlock(Handle);
    if (Status != CFE_SUCCESS) return Status;

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SRL_WriteUART(CFE_SRL_IO_Handle_t *Handle, const void *Data, size_t Size) {
    int32 Status;
    CFE_SRL_DevType_t DevType;
    if (Handle == NULL || Data == NULL) return CFE_SRL_BAD_ARGUMENT;

    DevType = CFE_SRL_GetHandleDevType(Handle);
    if (DevType != SRL_DEVTYPE_UART && DevType != SRL_DEVTYPE_RS422) return CFE_SRL_INVALID_TYPE;

    // Mutex Lock
    Status = CFE_SRL_MutexLock(Handle);
    if (Status != CFE_SUCCESS) return Status;

    // Write
    Status = CFE_SRL_Write(Handle, Data, Size);
    if (Status != CFE_SUCCESS) return Status;

    // Mutex Unlock
    Status = CFE_SRL_MutexUnlock(Handle);
    if (Status != CFE_SUCCESS) return Status;

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SRL_WriteCAN(CFE_SRL_IO_Handle_t *Handle, const void *Data, size_t Size, uint32_t Addr) {
    int32 Status;
    CFE_SRL_DevType_t DevType;
    struct can_frame Frame;

    if (Handle == NULL || Data == NULL) return CFE_SRL_BAD_ARGUMENT;

    DevType = CFE_SRL_GetHandleDevType(Handle);
    if (DevType != SRL_DEVTYPE_CAN) return CFE_SRL_INVALID_TYPE;

    if (Addr > 128) return -1; // Revise to `CAN_ADDR_ERR`, 128 to 29 bits max num


    // Mutex Lock
    Status = CFE_SRL_MutexLock(Handle);
    if (Status != CFE_SUCCESS) return Status;

    // Configure Frame
    Frame.can_id = Addr | CAN_EFF_FLAG; // Check if Frame use `29 bit addr` or not
    Frame.can_dlc = Size;
    memcpy(Frame.data, Data, Size);

    // Write
    Status = CFE_SRL_Write(Handle, &Frame, sizeof(Frame));
    if (Status != CFE_SUCCESS) return Status;

    // Mutex Unlock
    Status = CFE_SRL_MutexUnlock(Handle);
    if (Status != CFE_SUCCESS) return Status;

    return CFE_SUCCESS;
}


/**
 * Private Read function
 */

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SRL_ReadI2C(CFE_SRL_IO_Handle_t *Handle, const void *TxData, size_t TxSize, void *RxData, size_t RxSize, uint32_t Addr) {
    int32 Status;
    CFE_SRL_DevType_t DevType;

    if (Handle == NULL || RxData == NULL || TxData == NULL) return CFE_SRL_BAD_ARGUMENT;

    DevType = CFE_SRL_GetHandleDevType(Handle);
    if (DevType != SRL_DEVTYPE_I2C) return CFE_SRL_INVALID_TYPE;

    // Mutex Lock
    Status = CFE_SRL_MutexLock(Handle);
    if (Status != CFE_SUCCESS) return Status;

    // Transaction
    Status = CFE_SRL_TransactionI2C(Handle, TxData, TxSize, RxData, RxSize, Addr);
    if (Status != CFE_SUCCESS) return Status;

    // Mutex Unlock
    Status = CFE_SRL_MutexUnlock(Handle);
    if (Status != CFE_SUCCESS) return Status;

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SRL_ReadUART(CFE_SRL_IO_Handle_t *Handle, const void *TxData, size_t TxSize, void *RxData, size_t RxSize, uint32_t Timeout) {
    // write -> poll read
    int Status;
    CFE_SRL_DevType_t DevType;
    if (Handle == NULL || TxData == NULL || RxData == NULL) return CFE_SRL_BAD_ARGUMENT;

    DevType = CFE_SRL_GetHandleDevType(Handle);
    if (DevType != SRL_DEVTYPE_UART && DevType != SRL_DEVTYPE_RS422) return CFE_SRL_INVALID_TYPE;

    // Mutex Lock
    Status = CFE_SRL_MutexLock(Handle);
    if (Status != CFE_SUCCESS) return Status;

    // Write
    Status = CFE_SRL_Write(Handle, TxData, TxSize);
    if (Status != CFE_SUCCESS) return Status;

    // Need Some delay?
    //  OS_TaskDelay(100);
    
    // Poll Read
    Status = CFE_SRL_Read(Handle, RxData, RxSize, Timeout);
    if (Status != CFE_SUCCESS) return Status;

    // Mutex Unlock
    Status = CFE_SRL_MutexUnlock(Handle);
    if (Status != CFE_SUCCESS) return Status;

    return CFE_SUCCESS;
}

int32 CFE_SRL_ReadCAN(CFE_SRL_IO_Handle_t *Handle, const void *TxData, size_t TxSize, void *RxData, size_t RxSize, uint32_t Timeout, uint32_t Addr) {
    int32 Status;
    CFE_SRL_DevType_t DevType;

    if (Handle == NULL || TxData == NULL || RxData == NULL) return CFE_SRL_BAD_ARGUMENT;

    DevType = CFE_SRL_GetHandleDevType(Handle);
    if(DevType != SRL_DEVTYPE_CAN) return CFE_SRL_INVALID_TYPE;

    // Mutex Lock
    Status = CFE_SRL_MutexLock(Handle);
    if (Status != CFE_SUCCESS) return Status;

    // Write
    Status = CFE_SRL_Write(Handle, TxData, TxSize);
    if (Status != CFE_SUCCESS) return Status;

    // Need Some delay?
    // OS_TaskDelay(100);

    // Poll Read
    Status = CFE_SRL_Read(Handle, RxData, RxSize, Timeout);
    if (Status != CFE_SUCCESS) return Status;

    // Mutex Unlock
    Status = CFE_SRL_MutexUnlock(Handle);
    if (Status != CFE_SUCCESS) return Status;

    return CFE_SUCCESS;
}