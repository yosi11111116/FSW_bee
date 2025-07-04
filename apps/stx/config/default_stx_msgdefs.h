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
 * @file
 *   Specification for the STX command and telemetry
 *   message constant definitions.
 *
 *  For STX this is only the function/command code definitions
 */           
#ifndef STX_MSGDEFS_H
#define STX_MSGDEFS_H

#include "common_types.h"
#include "stx_fcncodes.h"


// telemetry send gs -> obc

typedef struct STX_DisplayParam_Payload
{
    uint32 ValU32;                            /**< 32 bit unsigned integer value */
    int16  ValI16;                            /**< 16 bit signed integer value */
    char   ValStr[STX_STRING_VAL_LEN]; /**< An example string */
} STX_DisplayParam_Payload_t;

typedef struct STX_Set_Payload
{
    uint8 data;                            /**< 32 bit unsigned integer value */
} STX_Set_Payload_t;

typedef struct STX_Set1_Payload
{
    uint8 data;                            /**< 32 bit unsigned integer value */
} STX_Set1_Payload_t;

typedef struct STX_Set2_Payload
{
    float data;                          /**< 32 bit unsigned integer value */
} STX_Set2_Payload_t;

typedef struct STX_Set3_Payload
{
    uint16 data;                           /**< 32 bit unsigned integer value */
} STX_Set3_Payload_t;

#pragma pack(push, 1)
typedef struct STX_Set4_Payload
{
    uint8_t  symbol_rate;
    uint8_t  transmit_power;
    uint8_t  modcod;
    uint8_t  roll_off;
    uint8_t  pilot_signal;
    uint8_t  fec_frame_size;
    uint16_t pretransmission_delay;
    float    center_frequency;
} STX_Set4_Payload_t;
#pragma pack(pop)

typedef struct STX_Set5_Payload
{
    uint8_t modulator_interface_type;  
    uint8_t lvds_io_type;              
} STX_Set5_Payload_t;

// typedef struct
// {
//     char filename[30];         // null-terminated string (최대 30 bytes)
//     uint32_t file_size_bytes;  // 4 bytes
// } STX_FS_FileEntry_t;
// typedef struct
// {
//     uint8_t  status;               // 1 byte
//     uint8_t  more_files_available; // 1 byte
//     uint16_t file_name_count;      // 2 byte

//     // 이후에 실제 수신시 동적으로 파싱
//     STX_FS_FileEntry_t entries[];  // 가변 길이 파일 정보
// } STX_FS_DIR_Payload_t;



#define STX_MAX_FILENAME_LEN 31  // 30 + NULL 종료
typedef struct {
    char filename[STX_MAX_FILENAME_LEN];  // Null-terminated string
} STX_DELFILE_Payload_t;
#define STX_FILENAME_MAX_LEN 31
typedef struct {
    char      filename[STX_FILENAME_MAX_LEN];  // null-terminated string (max 30 + '\0')
    uint32_t  file_size;                       // 파일 크기 (bytes)
} STX_CREATEFILE_Payload_t;

#define STX_FILE_WRITE_MAX_PAYLOAD 1472
typedef struct {
    uint16_t data_length;                           // 2-byte
    int32_t  file_handle;                           // 4-byte
    uint32_t packet_number;                         // 4-byte
    uint8_t  file_data[STX_FILE_WRITE_MAX_PAYLOAD]; // 1~1472 byte
} STX_WRITEFILE_Payload_t;

typedef struct {
    char filename[31];  // NULL terminated string (3~30 bytes)
} STX_OPENFILE_Payload_t;

typedef struct {
    int32_t file_handle;  // File handle from Open_File
} STX_READFILE_Payload_t;
#define MAX_FILENAME_LENGTH 31
typedef struct
{
    char filename[MAX_FILENAME_LENGTH];  // Null terminated string (3~30 chars)
} STX_SENDFILE_Payload_t;






typedef struct STX_Get_Payload
{
    uint16 data;                            /**< 32 bit unsigned integer value */
} STX_Get_Payload_t;
/*************************************************************************/
/*
** Type definition (Sample App housekeeping)
** command send stx_app <-> obc 
*/

typedef struct STX_HkTlm_Payload
{
    uint8 CommandErrorCounter;
    uint8 CommandCounter;
    uint8 spare[2];
} STX_HkTlm_Payload_t;


typedef struct STX_Tlm_Payload
{
    uint8 data;
    uint8 spare[2];
}STX_Tlm_Payload_t;

#define MAX_TM_DATASIZE     170
typedef struct{
    uint8 MaxBuffer[MAX_TM_DATASIZE];
}HYVRID_MaxTelemetryBuffer_t;

#endif
