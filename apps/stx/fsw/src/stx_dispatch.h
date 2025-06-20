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
 *
 * Main header file for the SAMPLE application
 */

#ifndef STX_DISPATCH_H
#define STX_DISPATCH_H

/*
** Required header files.
*/
#include "cfe.h"
#include "stx_msg.h"

void STX_TaskPipe(const CFE_SB_Buffer_t *SBBufPtr);
void STX_ProcessGroundCommand(const CFE_SB_Buffer_t *SBBufPtr);
void STX_Basic_ProcessGroundCommand(const CFE_SB_Buffer_t *SBBufPtr);
bool STX_VerifyCmdLength(const CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength);

#endif /* STX_DISPATCH_H */
