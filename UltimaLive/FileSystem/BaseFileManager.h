/** @file
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

#ifndef _I_FILE_MANAGER_H
#define _I_FILE_MANAGER_H

#include <cstdio>
#include <fstream>
#include <map>
#include <string>
#include <stdio.h>
#include <Windows.h>

#include "ClientFileHandleSet.h"
#include "BaseFileManager.h"
#include "..\Utils.h"
#include "..\ProgressBarDialog.h"
#include "..\ClassRegistration\SelfRegisteringClass.h"

class MapDefinition;
class LoginHandler;

/**
 * @class BaseFileManager
 *
 * @brief
 * The responsibility of the file manager is to handle the custom file formats that the client may employ. The main
 * reason for using a factory pattern here is because the client seems to be in a transistion phase where old
 * mul files are being converted to the new uop format.  The conversion has not happened all at once, so the factory
 * will allow the use of different file managers based on the client version.
 *
 * The file manager will redirect the client to open files from alternate folders, alter files that are on disk, 
 * create new map files, defragment files, and any other filesystem type tasks.
 */
class BaseFileManager
{
  public:
    BaseFileManager();

  virtual HANDLE WINAPI OnCreateFileA(
    __in      LPCSTR lpFileName,
    __in      DWORD dwDesiredAccess,
    __in      DWORD dwShareMode,
    __in_opt  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    __in      DWORD dwCreationDisposition,
    __in      DWORD dwFlagsAndAttributes,
    __in_opt  HANDLE hTemplateFile
  );

  virtual LPVOID WINAPI OnMapViewOfFile(
    __in  HANDLE hFileMappingObject,
    __in  DWORD dwDesiredAccess,
    __in  DWORD dwFileOffsetHigh,
    __in  DWORD dwFileOffsetLow,
    __in  SIZE_T dwNumberOfBytesToMap
    );

  virtual HANDLE WINAPI OnCreateFileMappingA(
    __in      HANDLE hFile,
    __in_opt  LPSECURITY_ATTRIBUTES lpAttributes,
    __in      DWORD flProtect,
    __in      DWORD dwMaximumSizeHigh,
    __in      DWORD dwMaximumSizeLow,
    __in_opt  LPCSTR lpName
  );

  virtual BOOL WINAPI OnCloseHandle(_In_  HANDLE hObject);

  virtual unsigned char* readStaticsBlock(uint32_t mapNumber, uint32_t blockNum, uint32_t& rNumberOfBytesOut);
  virtual bool writeStaticsBlock(uint8_t mapNumber, uint32_t blockNum, uint8_t* pBlockData, uint32_t length);
  virtual bool Initialize();

  /**
  * @brief Load a map
  *
  * @param mapNumber Map number
  */
  virtual void LoadMap(uint8_t mapNumber) = 0;

  virtual void InitializeShardMaps(std::string shardIdentifier, std::map<uint32_t, MapDefinition> definitions);
  virtual void onLogout();
  virtual bool updateLandBlock(uint8_t mapNumber, uint32_t blockNum, uint8_t* pData);
  virtual unsigned char* readLandBlock(uint8_t mapNumber, uint32_t blockNum);

  /**
   * @brief Seeks a land block in map file
   *
   * @param mapNumber Map Number
   * @param blockNum Block Number
   *
   * @return Pointer to land block memory
   */
  virtual unsigned char* seekLandBlock(uint8_t mapNumber, uint32_t blockNum) = 0;

  static void copyFile(std::string sourceFilePath, std::string destFilePath, ProgressBarDialog* pProgress);

  static const int MAP_MEMORY_SIZE = 100000000;     //!< Memory to allocate for the largest possible map file  
  static const int STAIDX_MEMORY_SIZE = 10000000;	//!< Memory to allocate for the largest possible statics index file
  static const int STATICS_MEMORY_SIZE = 200000000;	//!< Memory to allocate for the largets possible statics file

protected:
  std::map<std::string, ClientFileHandleSet*> m_files; //!< Maps the base map name to the set of handles for the corresponding files (map#.mul, staidx#.mul statics#.mul)
  uint8_t* m_pMapPool;        //!< Pointer to the memory pool used to load and unload maps
  uint8_t* m_pStaticsPool;    //!< Pointer to the memory pool used to load and unload statics
  uint8_t* m_pStaticsPoolEnd; //!< Pointer to the end of the statics memory pool
  uint8_t* m_pStaidxPool;     //!< Pointer to the statics index memory pool
  uint8_t* m_pStaidxPoolEnd;  //!< Pointer to the end of the statics index memory pool
  std::string m_shardIdentifier; //!< Unique shard identifier
  std::ofstream* m_pMapFileStream; //!< Pointer to map file stream
  std::ofstream* m_pStaidxFileStream; //!< Pointer to statics index file stream
  std::ofstream* m_pStaticsFileStream; //!< Pointer to statics file stream
  std::string getUltimaLiveSavePath();
  virtual bool createNewPersistentMap(std::string pathWithoutFilename, uint8_t mapNumber, uint32_t numHorizontalBlocks, uint32_t numVerticalBlocks);

  bool checkValidMemoryAllocated(void* pBuffer);

  ProgressBarDialog* m_pProgressDlg; //!< Pointer to the progress bar dialog
};
#endif