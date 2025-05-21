#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <csp/arch/csp_thread.h>
#include <csp/drivers/usart.h>
#include <csp/drivers/can_socketcan.h>
#include <csp/interfaces/csp_if_zmqhub.h>
#include <gs/ftp/client.h>
#include <gs/csp/csp.h>
#include <gs/csp/router.h>

#include "cfe_srl_csp.h"

const char *SatName = "COSMIC"; // Revise name according to specific misison

int CFE_SRL_RouteInitCSP(void) {
    int Status;

    csp_iface_t *InterfaceCAN = NULL;
    Status = csp_can_socketcan_open_and_add_interface(CSP_CAN_DEV_NAME, "CSPCAN0", 0, false, &InterfaceCAN);
    if (Status < 0) return -1; // Revise to `ROUTE_INIT_ERR`

    Status = csp_rtable_set(CSP_NODE_EPS, CSP_NODE_FIX_MASK, InterfaceCAN, CSP_NO_VIA_ADDRESS);
    if (Status != CSP_ERR_NONE) return -1; // Revise to `CSP_RTABLE_ERR`

    Status = csp_rtable_set(CSP_NODE_UTRX, CSP_NODE_FIX_MASK, InterfaceCAN, CSP_NO_VIA_ADDRESS);
    if (Status != CSP_ERR_NONE) return -1; // Revise to `CSP_RTABLE_ERR`

    Status = csp_rtable_set(CSP_NODE_STRX, CSP_NODE_FIX_MASK, InterfaceCAN, CSP_NO_VIA_ADDRESS);
    if (Status != CSP_ERR_NONE) return -1; // Revise to `CSP_RTABLE_ERR`

    Status = csp_rtable_set(CSP_NODE_GS_KISS, CSP_NODE_FIX_MASK, InterfaceCAN, CSP_NODE_UTRX);
    if (Status != CSP_ERR_NONE) return -1; // Revise to `CSP_RTABLE_ERR`

    Status = csp_rtable_set(CSP_NODE_GSTRX, CSP_NODE_FIX_MASK, InterfaceCAN, CSP_NODE_UTRX);
    if (Status != CSP_ERR_NONE) return -1; // Revise to `CSP_RTABLE_ERR`

    return CFE_SRL_OK;
}

int CFE_SRL_InitCSP(void) {
    int Status;

    csp_conf_t CspConfig = {
        .address            =   CSP_NODE_OBC, // Revise to Real OBC Node
        .hostname           =   "OBC",
        .model              =   SatName,
        .revision           =   "mozart",
        .conn_max           =   10, // Revise if needed. If external CSP device is too many.
        .conn_queue_length  =   10, // Connection queue's packet number.
        .fifo_length        =   25, // Router queue's packet number.
        .port_max_bind      =   32,
        .rdp_max_window     =   20,
        .buffers            =   10,
        .buffer_data_size   =   256,
        .conn_dfl_so        =   CSP_O_NONE
    };
    Status = csp_init(&CspConfig);
    if (Status != CSP_ERR_NONE) {
        CFE_ES_WriteToSysLog("%s: csp_init failed! CSP RC=%d\n",__func__, Status);
        return Status;
    }

    Status = csp_route_start_task(1024, GS_THREAD_PRIORITY_HIGH);
    if (Status != CSP_ERR_NONE) {
        CFE_ES_WriteToSysLog("%s: csp_route_start_task failed! CSP RC=%d\n", __func__, Status);
        return Status;
    }

    Status = CFE_SRL_RouteInitCSP();

}