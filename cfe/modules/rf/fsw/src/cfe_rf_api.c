/************************************************************************
 * Author : Hyeokjin Kweon
 * 
 * Last Modified : 2025 - 05 - 27
 * 
 * Brief : RF Comm. Core Module's API function 
 *         Used several App require RF function (CI, TO)
 ************************************************************************/

#include "cfe_rf_typedef.h"

/**
 * Global data
 */
static csp_socket_t *Socket = NULL;

void CFE_RF_CommandIngestTask(void);

void CFE_RF_CommandIngestInit(CFE_ES_TaskId_t *TaskIdPtr) {
    // csp_socket_t *Socket = NULL;
    int Status;
    Socket = csp_socket(CSP_O_NONE);
    if (Socket == NULL) {
        CFE_ES_WriteToSysLog("%s: csp_socket failed! NO RC\n", __func__);
        return;
    }

    Status = csp_bind(Socket, CFE_RF_PORT_PING);
    if (Status != CSP_ERR_NONE) {
        CFE_ES_WriteToSysLog("%s: csp_bind failed at Port: %d RC=%d\n", __func__, CFE_RF_PORT_PING, Status);
        return;
    }

    Status = csp_bind(Socket, CFE_RF_PORT_TC);
    if (Status != CSP_ERR_NONE) {
        CFE_ES_WriteToSysLog("%s: csp_bind failed at Port: %d RC=%d\n", __func__, CFE_RF_PORT_TC, Status);
        return;
    }

    Status = csp_bind(Socket, CFE_RF_PORT_FTP);
    if (Status != CSP_ERR_NONE) {
        CFE_ES_WriteToSysLog("%s: csp_bind failed at Port: %d RC=%d\n", __func__, CFE_RF_PORT_FTP, Status);
        return;
    }

    Status = csp_listen(Socket, 10);
    if (Status != CSP_ERR_NONE) {
        CFE_ES_WriteToSysLog("%s: csp_listen failed! RC=%d\n", __func__, Status);
        return;
    }

    CFE_ES_CreateChildTask(TaskIdPtr, "CI Task", CFE_RF_CommandIngestTask, CFE_ES_TASK_STACK_ALLOCATE, CI_TASK_STACK_SIZE(2), CI_TASK_PRIORITY(100), 0);
}

void CFE_RF_CommandIngestTask(void) {
    int32 Status;
    csp_conn_t *Connection = NULL;
    csp_packet_t *Packet = NULL;
    
    for (;;) {
        Connection = csp_accept(Socket, CSP_TIMEOUT(3));
        if (Connection == NULL) {
            CFE_ES_WriteToSysLog("%s: No incoming connection! NO RC\n", __func__);
            continue;
        }
        while ((Packet = csp_read(Connection, CSP_TIMEOUT(3))) != NULL) {
            int Port = csp_conn_dport(Connection);
            switch (Port) {
                case CFE_RF_PORT_PING: {
                    csp_service_handler(Connection, Packet);
                    break;
                }
                case CFE_RF_PORT_TC: {
                    CFE_SB_Buffer_t *BufPtr = NULL;

                    /**
                     * Allocate & Transmit Message buffer
                     */
                    BufPtr = CFE_SB_AllocateMessageBuffer(Packet->length);
                    memcpy(BufPtr, Packet->data, Packet->length);

                    /**
                     * Zero-copy transmit function
                     */
                    Status = CFE_SB_TransmitBuffer(BufPtr, false);

                    /**
                     * If failed, Release buffer and Transmit again via other function
                     */
                    if (Status != CFE_SUCCESS) {
                        CFE_SB_ReleaseMessageBuffer(BufPtr);
                        
                        /**
                         * Alternative Transmit function
                         */
                        Status = CFE_SB_TransmitMsg((CFE_MSG_Message_t *)Packet->data, false);
                        if (Status != CFE_SUCCESS) {
                            CFE_ES_WriteToSysLog("%s: Transmit message failed! RC=%d\n", __func__, Status);
                            return;
                        }
                    }
                    /* Free buffer & Remove dangled pointer */
                    csp_buffer_free(Packet);
                    Packet = NULL;
                    break;
                }
                case CFE_RF_PORT_FTP:
                    break;
            }
            csp_close(Connection);
        }
    }
    
}