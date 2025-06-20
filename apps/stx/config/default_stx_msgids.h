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
 *   STX Application Message IDs
 */
#ifndef STX_MSGIDS_H
#define STX_MSGIDS_H

#include "cfe_core_api_base_msgids.h"
#include "stx_topicids.h"

#define STX_CMD_MID     CFE_PLATFORM_CMD_TOPICID_TO_MIDV(CFE_MISSION_STX_CMD_TOPICID)
#define STX_SEND_HK_MID CFE_PLATFORM_CMD_TOPICID_TO_MIDV(CFE_MISSION_STX_SEND_HK_TOPICID)
#define STX_SEND_SET_MID  CFE_PLATFORM_CMD_TOPICID_TO_MIDV(CFE_MISSION_STX_SEND_SET_TOPICID)
#define STX_SEND_GET_MID  CFE_PLATFORM_CMD_TOPICID_TO_MIDV(CFE_MISSION_STX_SEND_GET_TOPICID)


#define STX_HK_TLM_MID  CFE_PLATFORM_TLM_TOPICID_TO_MIDV(CFE_MISSION_STX_HK_TLM_TOPICID)
#define STX_SET_TLM_MID  CFE_PLATFORM_TLM_TOPICID_TO_MIDV(CFE_MISSION_STX_SET_TLM_TOPICID)
#define STX_GET_TLM_MID  CFE_PLATFORM_TLM_TOPICID_TO_MIDV(CFE_MISSION_STX_GET_TLM_TOPICID)


#endif
