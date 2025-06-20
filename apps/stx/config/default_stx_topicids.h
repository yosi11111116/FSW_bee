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
 *   STX Application Topic IDs
 */
#ifndef STX_TOPICIDS_H
#define STX_TOPICIDS_H
// ground set inout value if you want to use
#define CFE_MISSION_STX_CMD_TOPICID       0x72
#define CFE_MISSION_STX_SEND_HK_TOPICID   0x73
#define CFE_MISSION_STX_SEND_SET_TOPICID    0x74
#define CFE_MISSION_STX_SEND_GET_TOPICID    0x75


// #define CFE_MISSION_STX_ESUP_INSIG_TOPICID    0x84
#define CFE_MISSION_STX_HK_TLM_TOPICID    0x76
#define CFE_MISSION_STX_SET_TLM_TOPICID    0x77
#define CFE_MISSION_STX_GET_TLM_TOPICID    0x78


#endif
