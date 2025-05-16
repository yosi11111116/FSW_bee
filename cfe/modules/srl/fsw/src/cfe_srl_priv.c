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

/**
 * Global data
 * 
 * Serial Handle for each srl comm.
 */
CFE_SRL_I2C_Handle_t *I2C0;
CFE_SRL_I2C_Handle_t *I2C1;
CFE_SRL_I2C_Handle_t *I2C2;
CFE_SRL_UART_Handle_t *UART;
CFE_SRL_RS422_Handle_t *RS422;
CFE_SRL_CAN_Handle_t *CAN0;

CFE_SRL_GPIO_Handle_t *GPIO[CFE_SRL_TOT_GPIO_NUM];

/**
 * GPIO Global Data - Must Check Real value and revise this
 */
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
    int Status;

    Status = CFE_SRL_PriorInit();
    if (Status != CFE_SRL_OK) {
        return Status; // Revise to `ERROR`
    }
    CFE_ES_WriteToSysLog("%s: Prior Initialized.", __func__);
    
    /**
     * Serial Comm. Init
     */
    Status = CFE_SRL_HandleInit(&I2C0, "UANT_I2C", "/dev/i2c-0", SRL_DEVTYPE_I2C, CFE_SRL_I2C0_MUTEX_IDX);
    if (Status != CFE_SRL_OK) {
        CFE_ES_WriteToSysLog("%s: I2C0 Initialization failed! RC=%d\n", __func__, Status);
        return -1;
    }
    CFE_ES_WriteToSysLog("%s: I2C0 Initialized. FD : %d | Name : %s | DevName : %s | MutexID : %u | Status : %u |", 
                    __func__, I2C0->FD, ((CFE_SRL_Global_Handle_t *)I2C0)->Name, ((CFE_SRL_Global_Handle_t *)I2C0)->DevName, ((CFE_SRL_Global_Handle_t *)I2C0)->MutexID, ((CFE_SRL_Global_Handle_t *)I2C0)->Status);


    Status = CFE_SRL_HandleInit(&CAN0, "CubeCAN", "can0", SRL_DEVTYPE_CAN, CFE_SRL_CAN0_MUTEX_IDX);
    if (Status != CFE_SRL_OK) {
        CFE_ES_WriteToSysLog("%s: CAN0 Initialization failed! RC=%d\n", __func__, Status);
        return -1;
    }
    CFE_ES_WriteToSysLog("%s: CAN0 Initialized. FD : %d | Name : %s | DevName : %s | MutexID : %u | Status : %u |", 
        __func__, CAN0->FD, ((CFE_SRL_Global_Handle_t *)CAN0)->Name, ((CFE_SRL_Global_Handle_t *)CAN0)->DevName, ((CFE_SRL_Global_Handle_t *)CAN0)->MutexID, ((CFE_SRL_Global_Handle_t *)CAN0)->Status);
    

    /**
     * GPIO Init
     */
    for (uint8 i = 0; i < CFE_SRL_TOT_GPIO_NUM; i++) {
        Status = CFE_SRL_GpioInit(GPIO[i], GpioPathArr[i], GpioLineArr[i], GpioNameArr[i], 0);
        if (Status != CFE_SRL_OK) {
            CFE_ES_WriteToSysLog("%s: GPIO%d Initialization failed! RC=%d\n", __func__, i, Status);
            return -1; // Revise to `GPIO_INIT_FAIL_ERR`
        }
    }
    CFE_ES_WriteToSysLog("%s: GPIO Initialized.", __func__);

    return CFE_SRL_OK;
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

    if (Handle == NULL || Data == NULL) return CFE_SRL_NULL_ERR;
    if (Addr > 128) return -1; // Revise to `I2C_ADDR_ERR`

    // Check dev type
    DevType = CFE_SRL_GetHandleDevType(Handle);
    if (DevType != SRL_DEVTYPE_I2C) return -1; // Revise `DEV_TYPE_ERR`

    // Mutex Lock
    Status = CFE_SRL_MutexLock(Handle);
    if (Status != CFE_SRL_OK) return Status;

    // Set Slave Addr
    Status = CFE_SRL_BasicIOCTL(Handle->FD, I2C_SLAVE, &Addr);
    if (Status < 0) {
        Handle->__errno = errno;
        return Status;
    }

    // Write
    Status = CFE_SRL_Write(Handle, Data, Size);
    if (Status != CFE_SRL_OK) return Status;

    // Mutex Unlock
    Status = CFE_SRL_MutexUnlock(Handle);
    if (Status != CFE_SRL_OK) return Status;

    return CFE_SRL_OK;
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
    if (Handle == NULL || Data == NULL) return CFE_SRL_NULL_ERR;

    DevType = CFE_SRL_GetHandleDevType(Handle);
    if (DevType != SRL_DEVTYPE_UART && DevType != SRL_DEVTYPE_RS422) return -1; // Revise `DEV_TYPE_ERR`

    // Mutex Lock
    Status = CFE_SRL_MutexLock(Handle);
    if (Status != CFE_SRL_OK) return Status;

    // Write
    Status = CFE_SRL_Write(Handle, Data, Size);
    if (Status != CFE_SRL_OK) return Status;

    // Mutex Unlock
    Status = CFE_SRL_MutexUnlock(Handle);
    if (Status != CFE_SRL_OK) return Status;

    return CFE_SRL_OK;
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

    if (Handle == NULL || Data == NULL) return CFE_SRL_NULL_ERR;

    DevType = CFE_SRL_GetHandleDevType(Handle);
    if (DevType != SRL_DEVTYPE_CAN) return -1; // Revise `DEV_TYPE_ERR`

    if (Addr > 128) return -1; // Revise to `CAN_ADDR_ERR`, 128 to 29 bits max num


    // Mutex Lock
    Status = CFE_SRL_MutexLock(Handle);
    if (Status != CFE_SRL_OK) return Status;

    // Configure Frame
    Frame.can_id = Addr | CAN_EFF_FLAG; // Check if Frame use `29 bit addr` or not
    Frame.can_dlc = Size;
    memcpy(Frame.data, Data, Size);

    // Write
    Status = CFE_SRL_Write(Handle, &Frame, sizeof(Frame));
    if (Status != CFE_SRL_OK) return Status;

    // Mutex Unlock
    Status = CFE_SRL_MutexUnlock(Handle);
    if (Status != CFE_SRL_OK) return Status;

    return CFE_SRL_OK;
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

    if (Handle == NULL || RxData == NULL || TxData == NULL) return CFE_SRL_NULL_ERR;

    DevType = CFE_SRL_GetHandleDevType(Handle);
    if (DevType != SRL_DEVTYPE_I2C) return -1; // Revise `DEV_TYPE_ERR`

    // Mutex Lock
    Status = CFE_SRL_MutexLock(Handle);
    if (Status != CFE_SRL_OK) return Status;

    // Transaction
    Status = CFE_SRL_TransactionI2C(Handle, TxData, TxSize, RxData, RxSize, Addr);
    if (Status != CFE_SRL_OK) return Status;

    // Mutex Unlock
    Status = CFE_SRL_MutexUnlock(Handle);
    if (Status != CFE_SRL_OK) return Status;

    return CFE_SRL_OK;
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
    if (Handle == NULL || TxData == NULL || RxData == NULL) return CFE_SRL_NULL_ERR;

    DevType = CFE_SRL_GetHandleDevType(Handle);
    if (DevType != SRL_DEVTYPE_UART && DevType != SRL_DEVTYPE_RS422) return -1; // Revise `DEV_TYPE_ERR`

    // Mutex Lock
    Status = CFE_SRL_MutexLock(Handle);
    if (Status != CFE_SRL_OK) return Status;

    // Write
    Status = CFE_SRL_Write(Handle, TxData, TxSize);
    if (Status != CFE_SRL_OK) return Status;

    // Need Some delay?
    //  OS_TaskDelay(100);
    
    // Poll Read
    Status = CFE_SRL_Read(Handle, RxData, RxSize, Timeout);
    if (Status != CFE_SRL_OK) return Status;

    // Mutex Unlock
    Status = CFE_SRL_MutexUnlock(Handle);
    if (Status != CFE_SRL_OK) return Status;

    return CFE_SRL_OK;
}

int32 CFE_SRL_ReadCAN(CFE_SRL_IO_Handle_t *Handle, const void *TxData, size_t TxSize, void *RxData, size_t RxSize, uint32_t Timeout, uint32_t Addr) {
    int32 Status;
    CFE_SRL_DevType_t DevType;

    if (Handle == NULL || TxData == NULL || RxData == NULL) return CFE_SRL_NULL_ERR;

    DevType = CFE_SRL_GetHandleDevType(Handle);
    if(DevType != SRL_DEVTYPE_CAN) return -1; // Revise `DEV_TYPE_ERR`

    // Mutex Lock
    Status = CFE_SRL_MutexLock(Handle);
    if (Status != CFE_SRL_OK) return Status;

    // Write
    Status = CFE_SRL_Write(Handle, TxData, TxSize);
    if (Status != CFE_SRL_OK) return Status;

    // Need Some delay?
    // OS_TaskDelay(100);

    // Poll Read
    Status = CFE_SRL_Read(Handle, RxData, RxSize, Timeout);
    if (Status != CFE_SRL_OK) return Status;

    // Mutex Unlock
    Status = CFE_SRL_MutexUnlock(Handle);
    if (Status != CFE_SRL_OK) return Status;

    return CFE_SRL_OK;
}