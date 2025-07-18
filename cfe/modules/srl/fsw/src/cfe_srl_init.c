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
 * 0 : UART Handle
 * 1 : RS422 Handle
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
	/* UART Init */
	Status = CFE_SRL_HandleInit(&Handles[CFE_SRL_UART_HANDLE_INDEXER], "UART", "/dev/ttyS0", SRL_DEVTYPE_UART, CFE_SRL_UART_HANDLE_INDEXER, 115200, 0);
	if (Status != CFE_SUCCESS) {
		CFE_ES_WriteToSysLog("%s: UART Initialization failed! RC=%d\n", __func__, Status);
		return CFE_SRL_UART_INIT_ERR;
	}
	CFE_ES_WriteToSysLog("%s: UART Initialized. FD=%d || DevName=%s\n", __func__, Handles[CFE_SRL_UART_HANDLE_INDEXER]->FD, ((CFE_SRL_Global_Handle_t *)Handles[CFE_SRL_UART_HANDLE_INDEXER])->DevName);

	/* RS422 Init */
	Status = CFE_SRL_HandleInit(&Handles[CFE_SRL_RS422_HANDLE_INDEXER], "RS422", "/dev/ttyS2", SRL_DEVTYPE_UART, CFE_SRL_RS422_HANDLE_INDEXER, 250000, 0);
	if (Status != CFE_SUCCESS) {
		CFE_ES_WriteToSysLog("%s: RS422 Initialization failed! RC=%d\n", __func__, Status);
		return CFE_SRL_RS422_INIT_ERR;
	}
	CFE_ES_WriteToSysLog("%s: RS422 Initialized. FD=%d || DevName=%s\n", __func__, Handles[CFE_SRL_RS422_HANDLE_INDEXER]->FD, ((CFE_SRL_Global_Handle_t *)Handles[CFE_SRL_RS422_HANDLE_INDEXER])->DevName);

return CFE_SUCCESS;
}
