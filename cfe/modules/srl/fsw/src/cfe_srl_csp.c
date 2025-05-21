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

/**
 * Indexed by CSP Node number.
 */
CFE_SRL_CSP_Node_Config_t *NodeConfig[32] = {0}; // Not mandatory

int CFE_SRL_RouteInitCSP(void) {
    int Status;

    csp_iface_t *InterfaceCAN = NULL;

    Status = csp_route_start_task(CSP_TASK_STACK_SIZE(1), GS_THREAD_PRIORITY_HIGH);
    if (Status != CSP_ERR_NONE) {
        CFE_ES_WriteToSysLog("%s: csp_route_start_task failed! CSP RC=%d\n", __func__, Status);
        return Status;
    }
    InterfaceCAN = csp_can_socketcan_init(CSP_CAN_DEV_NAME, 0, false);
    if (InterfaceCAN == NULL) {
        CFE_ES_WriteToSysLog("%s: CSP Socket CAN Init failed! NO RC", __func__);
        return -1; // Revise to `CSP_CAN_INIT_ERR`
    }

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

int CFE_SRL_NodeConfigCSP(uint8_t Node, uint8_t Priority, uint32_t Timeout, uint32_t Options) {
    if (NodeConfig[Node] == NULL) {
        NodeConfig[Node] = (CFE_SRL_CSP_Node_Config_t *)malloc(sizeof(CFE_SRL_CSP_Node_Config_t));
        memset(NodeConfig[Node], 0, sizeof(CFE_SRL_CSP_Node_Config_t));
    }

    NodeConfig[Node]->Priority = Priority;
    NodeConfig[Node]->Timeout = Timeout;
    NodeConfig[Node]->Options = Options;

    return CFE_SRL_OK;
}

int CFE_SRL_GetNodeConfigCSP(uint8_t Node, CFE_SRL_CSP_Node_Config_t *Config) {
    if (NodeConfig[Node] == NULL) return CFE_SRL_NOT_FOUND;

    Config = NodeConfig[Node];
    return CFE_SRL_OK;
}

/**
 * CSP Integrated Initialization function
 * Called by Early Init function of CFE SRL module
 */
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

    /**
     * Init CSP CAN and Register Routing Configuration
     */
    Status = CFE_SRL_RouteInitCSP();
    if (Status != CFE_SRL_OK) {
        CFE_ES_WriteToSysLog("%s: CSP Route Init failed! CSP RC=%d\n", __func__, Status);
        return -1; // Revise to `ROUTE_INIT_ERR`
    }


    /**
     * Register Node Configuration to each Node.
     * If Node appended(or revised), insert(or revise) the function.
     */
    CFE_SRL_NodeConfigCSP(CSP_NODE_EPS, CSP_PRIO_NORM, CSP_TIMEOUT(1), CSP_O_CRC32);
    CFE_SRL_NodeConfigCSP(CSP_NODE_UTRX, CSP_PRIO_NORM, CSP_TIMEOUT(1), CSP_O_CRC32);
    CFE_SRL_NodeConfigCSP(CSP_NODE_STRX, CSP_PRIO_NORM, CSP_TIMEOUT(1), CSP_O_CRC32);

    CFE_SRL_NodeConfigCSP(CSP_NODE_GS_KISS, CSP_PRIO_HIGH, CSP_TIMEOUT(3), CSP_O_CRC32);
    CFE_SRL_NodeConfigCSP(CSP_NODE_GSTRX, CSP_PRIO_HIGH, CSP_TIMEOUT(3), CSP_O_CRC32);

    return CFE_SRL_OK;
}

int CFE_SRL_TransactionCSP(uint8_t Node, uint8_t Port, const void *TxData, int TxSize, void *RxData, int RxSize) {
    // If there is no reply, Put `RxSize = 0`
    int Status;
    if (NodeConfig[Node] == NULL || TxData == NULL || RxData == NULL) return CFE_SRL_NULL_ERR;

    Status = csp_transaction_w_opts(NodeConfig[Node]->Priority, Node, Port, NodeConfig[Node]->Timeout, TxData, TxSize, RxData, RxSize, NodeConfig[Node]);
    if (Status == 0) return Status; // Revise to `CSP_TRANSACTION_ERR`

    // 1 or `reply size` on success
    return Status;
}