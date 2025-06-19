/* Auto-Generated file. Never change this code! */
/**
 * Required header files
*/
#include "cfe_srl_module_all.h"

/**
 * Global data
*/
CFE_SRL_IO_Handle_t *Handles[CFE_SRL_GNRL_DEVICE_NUM];
/**************************************************
 * Index of Each device
 * 0 : I2C0 Handle
 * 1 : I2C2 Handle
 * 2 : SPI0 Handle
 **************************************************/

CFE_SRL_GPIO_Handle_t GPIO[CFE_SRL_TOT_GPIO_NUM];


/************************************************************************
 * Early Initialization function executed at cFE ES
 * Append object to `cfe_es_objtab.c`
 * Declaration is located at
 * `cfe/modules/core_private/fsw/inc/cfe_srl_core_internal.h`
 ************************************************************************/
int32 CFE_SRL_EarlyInit(void) {
	int32 Status;

	Status = CFE_SRL_PriorInit();
	if(Status != CFE_SUCCESS) return Status;
	CFE_ES_WriteToSysLog("%s: Prior Initialized.", __func__);
	/**************************************************
	 * Serial Comm. Init
 	 * Only `ready == true` interface is initialized
	 **************************************************/
	/* I2C0 Init */
	Status = CFE_SRL_HandleInit(&Handles[CFE_SRL_I2C0_HANDLE_INDEXER], "I2C0", "/dev/i2c-0", SRL_DEVTYPE_I2C, CFE_SRL_I2C0_HANDLE_INDEXER, 0, 0);
	Handles[CFE_SRL_I2C0_HANDLE_INDEXER]->Func.TxFunc = CFE_SRL_WriteGenericI2C;
	Handles[CFE_SRL_I2C0_HANDLE_INDEXER]->Func.RxFunc = CFE_SRL_ReadGenericI2C;
	if (Status != CFE_SUCCESS) {
		CFE_ES_WriteToSysLog("%s: I2C0 Initialization failed! RC=%d\n", __func__, Status);
		return CFE_SRL_I2C0_INIT_ERR;
	}
	CFE_ES_WriteToSysLog("%s: I2C0 Initialized. FD=%d || DevName=%s\n", __func__, Handles[CFE_SRL_I2C0_HANDLE_INDEXER]->FD, ((CFE_SRL_Global_Handle_t *)Handles[CFE_SRL_I2C0_HANDLE_INDEXER])->DevName);

	/* I2C2 Init */
	Status = CFE_SRL_HandleInit(&Handles[CFE_SRL_I2C2_HANDLE_INDEXER], "I2C2", "/dev/i2c-2", SRL_DEVTYPE_I2C, CFE_SRL_I2C2_HANDLE_INDEXER, 0, 0);
	Handles[CFE_SRL_I2C2_HANDLE_INDEXER]->Func.TxFunc = CFE_SRL_WriteGenericI2C;
	Handles[CFE_SRL_I2C2_HANDLE_INDEXER]->Func.RxFunc = CFE_SRL_ReadGenericI2C;
	if (Status != CFE_SUCCESS) {
		CFE_ES_WriteToSysLog("%s: I2C2 Initialization failed! RC=%d\n", __func__, Status);
		return CFE_SRL_I2C2_INIT_ERR;
	}
	CFE_ES_WriteToSysLog("%s: I2C2 Initialized. FD=%d || DevName=%s\n", __func__, Handles[CFE_SRL_I2C2_HANDLE_INDEXER]->FD, ((CFE_SRL_Global_Handle_t *)Handles[CFE_SRL_I2C2_HANDLE_INDEXER])->DevName);

	/* GPIO STX_EN Init */
	Status = CFE_SRL_GpioInit(&GPIO[CFE_SRL_STX_EN_GPIO_INDEXER], "/dev/gpiochip2", 3, "STX_EN", 0);
	if (Status != CFE_SUCCESS) {
		CFE_StatusString_t str;
		CFE_ES_StatusToString(Status, &str);
		CFE_ES_WriteToSysLog("%s: GPIO STX_EN Initialization failed! RC=%s\n", __func__, str);
		return CFE_SRL_STX_EN_INIT_ERR;
	}

	/* SPI0 Init */
	Status = CFE_SRL_HandleInit(&Handles[CFE_SRL_SPI0_HANDLE_INDEXER], "SPI0", "/dev/spidev0.0", SRL_DEVTYPE_SPI, CFE_SRL_SPI0_HANDLE_INDEXER, 8000000, 0);
	Handles[CFE_SRL_SPI0_HANDLE_INDEXER]->Func.TxFunc = CFE_SRL_WriteGenericSPI;
	Handles[CFE_SRL_SPI0_HANDLE_INDEXER]->Func.RxFunc = CFE_SRL_ReadGenericSPI;
	if (Status != CFE_SUCCESS) {
		CFE_ES_WriteToSysLog("%s: SPI0 Initialization failed! RC=%d\n", __func__, Status);
		return CFE_SRL_SPI0_INIT_ERR;
	}
	CFE_ES_WriteToSysLog("%s: SPI0 Initialized. FD=%d || DevName=%s\n", __func__, Handles[CFE_SRL_SPI0_HANDLE_INDEXER]->FD, ((CFE_SRL_Global_Handle_t *)Handles[CFE_SRL_SPI0_HANDLE_INDEXER])->DevName);


	Status = CFE_SRL_InitCSP();
	if (Status != CFE_SUCCESS) {
		CFE_ES_WriteToSysLog("%s: CSP_Init failed! RC=0x%08X\n", __func__, Status);
	}
	CFE_ES_WriteToSysLog("%s: CSP Initialized.\n", __func__);
return CFE_SUCCESS;
}
