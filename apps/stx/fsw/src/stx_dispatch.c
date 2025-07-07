/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/**
 * \file
 *   This file contains the source code for the Sample App.
 */

/*
** Include Files:
*/
#include "stx.h"
#include "stx_dispatch.h"
#include "stx_cmds.h"
#include "stx_eventids.h"
#include "stx_msgids.h"
#include "stx_msg.h"
#include "esup.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* Verify command packet length                                               */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
bool STX_VerifyCmdLength(const CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength)
{
    bool              result       = true;
    size_t            ActualLength = 0;
    CFE_SB_MsgId_t    MsgId        = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t FcnCode      = 0;

    CFE_MSG_GetSize(MsgPtr, &ActualLength);

    /*
    ** Verify the command packet length.
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_MSG_GetMsgId(MsgPtr, &MsgId);
        CFE_MSG_GetFcnCode(MsgPtr, &FcnCode);

        CFE_EVS_SendEvent(STX_CMD_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid Msg length: ID = 0x%X,  CC = %u, Len = %u, Expected = %u",
                          (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)FcnCode, (unsigned int)ActualLength,
                          (unsigned int)ExpectedLength);

        result = false;

        STX_Data.ErrCounter++;
    }

    return result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* SAMPLE ground commands                                                     */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/

/* basic */
void STX_Basic_ProcessGroundCommand(const CFE_SB_Buffer_t *SBBufPtr){
    CFE_MSG_FcnCode_t CommandCode = 0;
    CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &CommandCode);

    /*
    ** Process SAMPLE app ground commands
    */
    switch (CommandCode)
    {
        case STX_NOOP_CC:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_NoopCmd_t)))
            {
                STX_NoopCmd((const STX_NoopCmd_t *)SBBufPtr);
            }
            break;

        case STX_RESET_COUNTERS_CC:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_ResetCountersCmd_t)))
            {
                STX_ResetCountersCmd((const STX_ResetCountersCmd_t *)SBBufPtr);
            }
            break;


        case STX_DISPLAY_PARAM_CC:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_DisplayParamCmd_t)))
            {
                STX_DisplayParamCmd((const STX_DisplayParamCmd_t *)SBBufPtr);
            }
            break;
            
        default:
            CFE_EVS_SendEvent(STX_CC_ERR_EID, CFE_EVS_EventType_ERROR, "Invalid ground command code: CC = %d",
                              CommandCode);
            break;
    }
}

/* set */
void STX_ProcessGroundCommand(const CFE_SB_Buffer_t *SBBufPtr){
    CFE_MSG_FcnCode_t CommandCode = 0;
    
    int8_t rxlength_g = 1;
    int16_t status;
    uint16_t command;
    uint16_t type;

    void *rxdata = (void *)&STX_Data.SetTlm.Payload.data;
    uint8_t rxlength = 1;

    CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &CommandCode);
    OS_printf("set: %zu\n", sizeof(STX_Set_t));
    printf("Sbb: %zu \n", sizeof(CFE_SB_Buffer_t));
    // const STX_Set_t *cmd = (const STX_Set_t *)SBBufPtr;
    switch (CommandCode)
    {
        case STX_SET_SYMBOLRATE:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Set_t)))
            {
                const STX_Set_t *cmd = (const STX_Set_t *)SBBufPtr;
                void *txdata = (void *)&cmd->Payload.data;
                uint16_t txlength = sizeof(cmd->Payload.data);

                status = ESUP_INSIG;
                command = CONFIG_CC_SET;
                type = CONFIG_TP_SYMBOLRATE;
                ESUP(status, command, type, txdata, txlength, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;

        case STX_SET_TRANSMITPW:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Set1_t)))
            {

                const STX_Set1_t *cmd = (const STX_Set1_t *)SBBufPtr;
                void *txdata = (void *)&cmd->Payload.data;
                uint16_t txlength = sizeof(cmd->Payload.data);

                status = ESUP_INSIG;
                command = CONFIG_CC_SET;
                type = CONFIG_TP_TRANSMITPW;

                ESUP(status, command, type, txdata, txlength, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;


        case STX_SET_CENTERFREQ:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Set2_t)))
            {
                const STX_Set2_t *cmd = (const STX_Set2_t *)SBBufPtr;
                void *txdata = (void *)&cmd->Payload.data;
                uint16_t txlength = sizeof(cmd->Payload.data);

                status = ESUP_INSIG;
                command = CONFIG_CC_SET;
                type = CONFIG_TP_CENTERFREQ;

                ESUP(status, command, type, txdata, txlength, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;


        case STX_SET_MODCOD:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Set_t)))
            {
                const STX_Set1_t *cmd = (const STX_Set1_t *)SBBufPtr;
                void *txdata = (void *)&cmd->Payload.data;
                uint16_t txlength = sizeof(cmd->Payload.data);

                status = ESUP_INSIG;
                command = CONFIG_CC_SET;
                type = CONFIG_TP_MODCOD;

                ESUP(status, command, type, txdata, txlength, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;


        case STX_SET_ROLLOFF:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Set_t)))
            {
                const STX_Set1_t *cmd = (const STX_Set1_t *)SBBufPtr;
                void *txdata = (void *)&cmd->Payload.data;
                uint16_t txlength = sizeof(cmd->Payload.data);

                status = ESUP_INSIG;
                command = CONFIG_CC_SET;
                type = CONFIG_TP_ROLLOFF;

                ESUP(status, command, type, txdata, txlength, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;


        case STX_SET_PILOTSIG:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Set_t)))
            {
                const STX_Set1_t *cmd = (const STX_Set1_t *)SBBufPtr;
                void *txdata = (void *)&cmd->Payload.data;
                uint16_t txlength = sizeof(cmd->Payload.data);

                status = ESUP_INSIG;
                command = CONFIG_CC_SET;
                type = CONFIG_TP_PILOTSIG;

                ESUP(status, command, type, txdata, txlength, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;

        case STX_SET_FECFRAMESZ:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Set_t)))
            {
                const STX_Set1_t *cmd = (const STX_Set1_t *)SBBufPtr;
                void *txdata = (void *)&cmd->Payload.data;
                uint16_t txlength = sizeof(cmd->Payload.data);

                status = ESUP_INSIG;
                command = CONFIG_CC_SET;
                type = CONFIG_TP_FECFRAMESZ;

                ESUP(status, command, type, txdata, txlength, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            
            }
            break;

        case STX_SET_PRETX_DELAY:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Set_t)))
            {
                const STX_Set3_t *cmd = (const STX_Set3_t *)SBBufPtr;
                void *txdata = (void *)&cmd->Payload.data;
                uint16_t txlength = sizeof(cmd->Payload.data);

                status = ESUP_INSIG;
                command = CONFIG_CC_SET;
                type = CONFIG_TP_PRETXSTUFFDEL;

                ESUP(status, command, type, txdata, txlength, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;

        case STX_SET_ALL_PRAMETERS:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Set_t)))
            {

                const STX_Set4_t *cmd = (const STX_Set4_t *)SBBufPtr;
                void *txdata = (void *)&cmd->Payload;
                uint16_t txlength = sizeof(cmd->Payload);

                status = ESUP_INSIG;
                command = CONFIG_CC_SET;
                type = CONFIG_TP_ALLPARAM;

                ESUP(status, command, type, txdata, txlength, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;

        case STX_SET_RS485BAUD:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Set_t)))
            {
                const STX_Set1_t *cmd = (const STX_Set1_t *)SBBufPtr;
                void *txdata = (void *)&cmd->Payload.data;
                uint16_t txlength = sizeof(cmd->Payload.data);

                status = ESUP_INSIG;
                command = CONFIG_CC_SET;
                type = CONFIG_TP_RS485BAUD;

                ESUP(status, command, type, txdata, txlength, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;

        case STX_SET_MODULATOR_DATA_INTERFACE:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Set5_t)))
            {
                const STX_Set5_t *cmd = (const STX_Set5_t *)SBBufPtr;
                void *txdata = (void *)&cmd->Payload;
                uint16_t txlength = sizeof(cmd->Payload);

                status = ESUP_INSIG;
                command = CONFIG_CC_SET;
                type = CONFIG_TP_MODULATORDTIFC;

                ESUP(status, command, type, txdata, txlength, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;

        case STX_FILESYS_CC_DIR:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Set_t)))
            {

                // const STX_FS_DIR_t *cmd = (const STX_FS_DIR_t *)SBBufPtr;
                // void *txdata = (void *)&cmd->Payload;
                // uint16_t txlength = sizeof(cmd->Payload);

                status = ESUP_INSIG;
                command = FILESYS_CC_DIR;
                type = FILESYS_TP_NA;

                ESUP(status, command, type, NULL, 0, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;

        case STX_FILESYS_CC_DIRNEXT:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Set_t)))
            {
                status = ESUP_INSIG;
                command = FILESYS_CC_DIRNEXT;
                type = FILESYS_TP_NA;

                ESUP(status, command, type, NULL, 0, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;

        case STX_FILESYS_CC_DELFILE:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Set_t)))
            {
                const STX_DELFILE_t *cmd = (const STX_DELFILE_t *)SBBufPtr;
                void *txdata = (void *)&cmd->Payload.filename;
                uint16_t txlength = sizeof(cmd->Payload.filename);
                // uint16_t txlength = strnlen(cmd->Payload.filename, STX_MAX_FILENAME_LEN) + 1;

                status = ESUP_INSIG;
                command = FILESYS_CC_DELFILE;
                type = FILESYS_TP_NA;

                ESUP(status, command, type, txdata, txlength, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;

        case STX_FILESYS_CC_DELALLFILE:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Set_t)))
            {
                status = ESUP_INSIG;
                command = FILESYS_CC_DELALLFILE;
                type = FILESYS_TP_NA;

                ESUP(status, command, type, NULL, 0, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;

        case STX_FILESYS_CC_CREATEFILE:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Set_t)))
            {
                const STX_CREATEFILE_t *cmd = (const STX_CREATEFILE_t *)SBBufPtr;
                void *txdata = (void *)&cmd->Payload.filename;
                uint16_t txlength = sizeof(cmd->Payload.filename);

                status = ESUP_INSIG;
                command = FILESYS_CC_CREATEFILE;
                type = FILESYS_TP_NA;

                ESUP(status, command, type, txdata, txlength, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;

        case STX_FILESYS_CC_WRITEFILE:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Set_t)))
            {
                const STX_WRITEFILE_t *cmd = (const STX_WRITEFILE_t *)SBBufPtr;
                void *txdata = (void *)&cmd->Payload;
                uint16_t txlength = sizeof(cmd->Payload);

                status = ESUP_INSIG;
                command = FILESYS_CC_WRITEFILE;
                type = FILESYS_TP_NA;

                ESUP(status, command, type, txdata, txlength, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;

        case STX_FILESYS_CC_OPENFILE:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Set_t)))
            {
                const STX_OPENFILE_t *cmd = (const STX_OPENFILE_t *)SBBufPtr;
                void *txdata = (void *)&cmd->Payload;
                uint16_t txlength = sizeof(cmd->Payload);

                status = ESUP_INSIG;
                command = FILESYS_CC_OPENFILE;
                type = FILESYS_TP_NA;

                ESUP(status, command, type, txdata, txlength, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;

        case STX_FILESYS_CC_READFILE:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Set_t)))
            {
                const STX_READFILE_t *cmd = (const STX_READFILE_t *)SBBufPtr;
                void *txdata = (void *)&cmd->Payload.file_handle;
                uint16_t txlength = sizeof(cmd->Payload.file_handle);

                status = ESUP_INSIG;
                command = FILESYS_CC_READFILE;
                type = FILESYS_TP_NA;

                ESUP(status, command, type, txdata, txlength, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;

        case STX_FILESYS_CC_SENDFILE:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Set_t)))
            {
                const STX_SENDFILE_t *cmd = (const STX_SENDFILE_t *)SBBufPtr;
                void *txdata = (void *)&cmd->Payload.filename;
                uint16_t txlength = sizeof(cmd->Payload.filename);

                status = ESUP_INSIG;
                command = FILESYS_CC_SENDFILE;
                type = FILESYS_TP_SENDFILE;

                ESUP(status, command, type, txdata, txlength, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;

        // case STX_FILESYS_CC_SENDFILEPI:
        //     if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Set_t)))
        //     {
        //         status = ESUP_INSIG;
        //         command = FILESYS_CC_SENDFILE;
        //         type = FILESYS_TP_SENDFILEPI;

        //         ESUP(status, command, type, txdata, txlength, rxdata, rxlength);
        //         ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
        //     }
        //     break;

        // case STX_FILESYS_CC_SENDFILERTI:
        //     if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Set_t)))
        //     {
        //         status = ESUP_INSIG;
        //         command = FILESYS_CC_SENDFILE;
        //         type = FILESYS_TP_SENDFILERTI;

        //         ESUP(status, command, type, txdata, txlength, rxdata, rxlength);
        //         ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
        //     }
        //     break;

        // case STX_FILESYS_CC_SENDFILEAI:
        //     if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Set_t)))
        //     {
        //         status = ESUP_INSIG;
        //         command = FILESYS_CC_SENDFILE;
        //         type = FILESYS_TP_SENDFILEAI;

        //         ESUP(status, command, type, txdata, txlength, rxdata, rxlength);
        //         ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
        //     }
        //     break;

        case STX_SYSCONF_CC_TRANSMITMODE:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Set_t)))
            {
                status = ESUP_INSIG;
                command = SYSCONF_CC_TRANSMITMODE;
                type = SYSCONF_TP_NA;

                ESUP(status, command, type, NULL, 0, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;

        case STX_SYSCONF_CC_IDLEMODE:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Set_t)))
            {
                status = ESUP_INSIG;
                command = SYSCONF_CC_IDLEMODE;
                type = SYSCONF_TP_NA;

                ESUP(status, command, type, NULL, 0, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;

        case STX_SYSCONF_CC_SAFESHUTDOWN:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Set_t)))
            {
                status = ESUP_INSIG;
                command = SYSCONF_CC_SAFESHUTDOWN;
                type = SYSCONF_TP_NA;

                ESUP(status, command, type, NULL, 0, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;


        // case STX_GETRES_CC_GETRES:
        //     if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Set_t)))
        //     {
        //         const STX_Set_t *cmd = (const STX_Set_t *)SBBufPtr;
        //         status  = ESUP_INSIG;
        //         command = GETRES_CC_GETRES;
        //         type    = CONFIG_TP_MODULATORDTIFC;

        //         ESUP(status, command, type, (void *)&cmd->Payload.data, (uint16_t)cmd->Payload.length);
        //     }
        //     break;
        /* default case already found during FC vs length test */
        case STX_SYSCONF_CC_UPDATEFW:
            // if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Set_t)))
            // {
            //     const STX_Set_t *cmd = (const STX_Set_t *)SBBufPtr;

            //     status  = ESUP_INSIG;
            //     command = SYSCONF_CC_UPDATEFW;
            //     type    = SYSCONF_TP_NA;

            //     ESUP(status, command, type, (void *)&cmd->Payload.data, (uint16_t)cmd->Payload.length);
            // }
            break;

        default:
            CFE_EVS_SendEvent(STX_CC_ERR_EID, CFE_EVS_EventType_ERROR, "Invalid ground command code: CC = %d",
                              CommandCode);
            break;


        CFE_SB_TimeStampMsg(CFE_MSG_PTR(STX_Data.SetTlm.TelemetryHeader));
        CFE_SB_TransmitMsg(CFE_MSG_PTR(STX_Data.SetTlm.TelemetryHeader), true);
    }
}

/* get */
void STX_ProcessRequestedTelemetry(const CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_MSG_FcnCode_t CommandCode = 0;
    uint8_t rxlength_g =1;
    uint8_t rxlength;
    int16_t status;
    uint16_t command;
    uint16_t type;

    CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &CommandCode);
    void *rxdata = (void *)&STX_Data.GetTlm.Payload.data;

    /*
    ** Process SAMPLE app ground commands
    */

    switch (CommandCode)
    {
        case STX_GET_SYMBOL_RATE:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Get_t))) {
                status = ESUP_INSIG;
                command = CONFIG_CC_GET;
                type = CONFIG_TP_SYMBOLRATE;
                rxlength = 1;
                ESUP(status, command, type, NULL, 0, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;

        case STX_GET_TX_POWER:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Get_t))) {
                status = ESUP_INSIG;
                command = CONFIG_CC_GET;
                type = CONFIG_TP_TRANSMITPW;
                rxlength = 1;
                ESUP(status, command, type, NULL, 0, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;

        case STX_GET_CENTER_FREQ:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Get_t))) {
                status = ESUP_INSIG;
                command = CONFIG_CC_GET;
                type = CONFIG_TP_CENTERFREQ;
                rxlength = 4;
                ESUP(status, command, type, NULL, 0, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;

        case STX_GET_MODCOD:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Get_t))) {
                status = ESUP_INSIG;
                command = CONFIG_CC_GET;
                type = CONFIG_TP_MODCOD;
                rxlength = 1;
                ESUP(status, command, type, NULL, 0, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;

        case STX_GET_ROLL_OFF:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Get_t))) {
                status = ESUP_INSIG;
                command = CONFIG_CC_GET;
                type = CONFIG_TP_ROLLOFF;
                rxlength = 1;
                ESUP(status, command, type, NULL, 0, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;

        case STX_GET_PILOT_SIGNAL:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Get_t))) {
                status = ESUP_INSIG;
                command = CONFIG_CC_GET;
                type = CONFIG_TP_PILOTSIG;
                rxlength = 1;
                ESUP(status, command, type, NULL, 0, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;

        case STX_GET_FEC_FRAME_SIZE:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Get_t))) {
                status = ESUP_INSIG;
                command = CONFIG_CC_GET;
                type = CONFIG_TP_FECFRAMESZ;
                rxlength = 1;
                ESUP(status, command, type, NULL, 0, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;

        case STX_GET_PRETX_DELAY:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Get_t))) {
                status = ESUP_INSIG;
                command = CONFIG_CC_GET;
                type = CONFIG_TP_PRETXSTUFFDEL;
                rxlength = 2;
                ESUP(status, command, type, NULL, 0, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;

        case STX_GET_ALL_PRAMETERS:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Get_t))) {
                status = ESUP_INSIG;
                command = CONFIG_CC_GET;
                type = CONFIG_TP_ALLPARAM;
                rxlength = 12;
                ESUP(status, command, type, NULL, 0, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;

        case STX_GET_REPORT:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Get_t))) {
                status = ESUP_INSIG;
                command = CONFIG_CC_GET;
                type = STATUS_TP_SIMPLE_REPORT;
                rxlength = 12;
                ESUP(status, command, type, NULL, 0, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;

        case STX_GET_MODULATOR_DATA_INTERFACE:
            if (STX_VerifyCmdLength(&SBBufPtr->Msg, sizeof(STX_Get_t))) {
                status = ESUP_INSIG;
                command = CONFIG_CC_GET;
                type = CONFIG_TP_MODULATORDTIFC;
                rxlength = 2;
                ESUP(status, command, type, NULL, 0, rxdata, rxlength);
                ESUP(status, GETRES_CC_GETRES, command, &type, sizeof(type), rxdata, rxlength_g);
            }
            break;

        /* default case already found during FC vs length test */
        default:
            CFE_EVS_SendEvent(STX_CC_ERR_EID, CFE_EVS_EventType_ERROR, "Invalid ground command code: CC = %d", CommandCode);
            break;

        CFE_SB_TimeStampMsg(CFE_MSG_PTR(STX_Data.GetTlm.TelemetryHeader));
        CFE_SB_TransmitMsg(CFE_MSG_PTR(STX_Data.GetTlm.TelemetryHeader), true);

    }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet 1that is received on the SAMPLE   */
/*     command pipe.                                                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void STX_TaskPipe(const CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;

    CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MsgId);
    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        case STX_CMD_MID:
            STX_Basic_ProcessGroundCommand(SBBufPtr);
            break;
        case STX_SEND_SET_MID:
            STX_ProcessGroundCommand(SBBufPtr);
            break;
        case STX_SEND_GET_MID:
            STX_ProcessRequestedTelemetry(SBBufPtr);
            break;
        case STX_SEND_HK_MID:
            STX_SendHkCmd((const STX_SendHkCmd_t *)SBBufPtr);
            break;

        default:
            CFE_EVS_SendEvent(STX_MID_ERR_EID, CFE_EVS_EventType_ERROR,
                              "SAMPLE: invalid command packet,MID = 0x%x", (unsigned int)CFE_SB_MsgIdToValue(MsgId));
            break;
    }
}


