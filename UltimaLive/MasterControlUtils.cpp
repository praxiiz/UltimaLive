/* Copyright (C) 2012 Matthew Geyer
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

#include "MasterControlUtils.h"

unsigned char MasterControlUtils::g_functionStartSig1[] = { 0x55, 0x8B, 0xEC };
unsigned char MasterControlUtils::g_functionStartSig2[] = { 0x90, 0x90, 0x6A };


void* MasterControlUtils::FindSignatureOffsetBackwards(void* pData, unsigned int searchLen, unsigned char *sigBuffer, unsigned int sigLen)
{
  unsigned char* pBuffer = static_cast<unsigned char *>(static_cast<unsigned char*>(pData) - searchLen - 1);

	BOOL found = FALSE;
	for (unsigned int x = searchLen; x > 0; --x)
	{
		for(unsigned int y = 0; y < sigLen; ++y)
		{
			if (sigBuffer[y] == 0xCC || pBuffer[x + y] == sigBuffer[y])
      {
				found = TRUE;
      }
			else
			{
				found = FALSE;
				break;
			}
		}

		if (found)
		{
			return pBuffer + x;
		}
	}

	return 0;
}

void* MasterControlUtils::FindFunctionCall(void* pBuffer, int bufferSize, unsigned char* pSignature, int signatureSize)
{
  void* pAddress = 0;

  void* pSigAddress = MasterControlUtils::FindSignatureOffset(pBuffer, bufferSize, pSignature, signatureSize);

  if (pSigAddress != 0)
  {
    pAddress = MasterControlUtils::FindSignatureOffsetBackwards(pSigAddress, 100, g_functionStartSig1, sizeof(g_functionStartSig1));
    if (pAddress == 0)
    {
      pAddress = MasterControlUtils::FindSignatureOffsetBackwards(pSigAddress, 100, g_functionStartSig2, sizeof(g_functionStartSig2));
      if (pAddress != 0)
      {
        pAddress = static_cast<char*>(pAddress) + 2;
      }
    }
  }

  return pAddress;
}

void* MasterControlUtils::FindSignature (void* pBuffer, int bufferSize, unsigned char* pSignature, int signatureSize, int offset, int& structureSizeOut, int structureSizeOffset)
{
  unsigned char* retValue = 0;
  unsigned char* pAddress = static_cast<unsigned char *>(FindSignatureOffset(pBuffer, bufferSize, pSignature, signatureSize));

  if (pAddress != 0)
  {
    retValue = reinterpret_cast<unsigned char*>(*reinterpret_cast<unsigned int*>(pAddress + offset));
    structureSizeOut = *reinterpret_cast<int *>(pAddress + structureSizeOffset);
  }

  return retValue;
}

void* MasterControlUtils::FindSignature (void* pBuffer, int bufferSize, unsigned char* pSignature, int signatureSize, int offset)
{
  unsigned char* retValue = 0;

  unsigned char* pAddress = static_cast<unsigned char *>(FindSignatureOffset(pBuffer, bufferSize, pSignature, signatureSize));

  if (pAddress != 0)
  {
    retValue = reinterpret_cast<unsigned char*>(*reinterpret_cast<unsigned int*>(pAddress + offset));
  }

  return retValue;
}

/*********************************************************************************************************************************************
 * Search memory for specified byte signature.  Note that 0xCC is a wildcard byte, it matches anything.  A more proper way to do this is
 * one pointer to a signature and another pointer to wildcard flags, but this is simpler and works well.
 * Returns a pointer to the beginning of the signature match.  Returns 0 on no match.
 *********************************************************************************************************************************************/
void* MasterControlUtils::FindSignatureOffset(void* pBuffer, unsigned int bufferLen, unsigned char *sigBuffer, unsigned int sigLen)
{
	bool found = false;
	for (unsigned int x = 0; x < bufferLen - sigLen; x++)
	{
		for(unsigned int y = 0; y < sigLen; y++)
		{
			if (sigBuffer[y] == 0xCC || static_cast<unsigned char*>(pBuffer)[x + y] == sigBuffer[y])
      {
				found = true;
      }
			else
			{
				found = false;
				break;
			}
		}
		if (found)
		{
			return static_cast<unsigned char*>(pBuffer) + x;
		}
	}

	return 0;
}
