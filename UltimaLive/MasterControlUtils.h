/* @file 
 *
 * Copyright (C) 2012 Matthew Geyer
 * Copyright (C) 2016 UltimaLive
 *
 * This file is part of MasterControl.
 * 
 * MasterControl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * MasterControl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with MasterControl.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef _MASTER_CONTROL_UTILS_H
#define _MASTER_CONTROL_UTILS_H

#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include "Debug.h"
#include "Client.h"

/**
 * @class MasterControlUtils
 *
 * @brief Utility class for searching for a process's internal structures
 */
class MasterControlUtils
{
  public:
    static void* FindSignatureOffset(void* pBuffer, unsigned int bufferLen, unsigned char* sigBuffer, unsigned int sigLen);
    static void* FindFunctionCall(void* pBuffer, int bufferSize, unsigned char* pSignature, int signatureSize);
    static void* FindSignatureOffsetBackwards(void* pData, unsigned int searchLen, unsigned char *sigBuffer, unsigned int sigLen);
    static void* FindSignature (void* pBuffer, int bufferSize, unsigned char* pSignature, int signatureSize, int offset, int& structureSizeOut, int structureSizeOffset);
    static void* FindSignature (void* pBuffer, int bufferSize, unsigned char* pSignature, int signatureSize, int offset);

  private:
    static unsigned char g_functionStartSig1[]; //!< Function signature
    static unsigned char g_functionStartSig2[]; //!< Function signature
};

#endif