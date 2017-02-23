/**
 * @file
 *
 * Copyright(c) 2016 UltimaLive
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#ifndef _FILE_MANAGER_7_0_29_2_H
#define _FILE_MANAGER_7_0_29_2_H

#include "..\BaseFileManager.h"
#include "..\Uop\UopStructs.h"
#include "..\Uop\UopUtility.h"
#include "..\..\Utils.h"
#include "..\..\LocalPeHelper32.hpp"

#include <Windows.h>
#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <stdint.h>

/**
 * @class FileManager_7_0_29_2
 *
 * @brief FileManager class for clients using UOP files
 */
class FileManager_7_0_29_2 : public BaseFileManager
{
  public:
    FileManager_7_0_29_2();

    LPVOID WINAPI OnMapViewOfFile(
      __in  HANDLE hFileMappingObject,
      __in  DWORD dwDesiredAccess,
      __in  DWORD dwFileOffsetHigh,
      __in  DWORD dwFileOffsetLow,
      __in  SIZE_T dwNumberOfBytesToMap
    );
    
    void InitializeShardMaps(std::string shardIdentifier, std::map<uint32_t, MapDefinition> definitions);

    bool Initialize();
    void LoadMap(uint8_t mapNumber);
  protected:
    std::map<uint32_t, FileEntry*> m_fileEntries; //!< file entries in the UOP file
    std::map<std::string, uint32_t> m_neededFiles; //!< files needed by this File Manager
    unsigned char* seekLandBlock(uint8_t mapNumber, uint32_t blockNum);

    void parseMapFile(std::string filename);
};
#endif