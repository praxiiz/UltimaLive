/* @file
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

#include "FileManager.h"
#include <cstdio>

#include "..\Uop\UopUtility.h"
#include "..\..\Maps\MapDefinition.h"

/**
 * @brief FileManager constructor
 */
FileManager::FileManager()
  : BaseFileManager(),
    m_neededFiles()
{
  m_neededFiles["map0.mul"] = 0;
  m_neededFiles["staidx0.mul"] = 0;
  m_neededFiles["statics0.mul"] = 0;

  m_neededFiles["map1.mul"] = 0;
  m_neededFiles["staidx1.mul"] = 0;
  m_neededFiles["statics1.mul"] = 0;
  
  m_neededFiles["map2.mul"] = 0;
  m_neededFiles["staidx2.mul"] = 0;
  m_neededFiles["statics2.mul"] = 0;
  
  m_neededFiles["map3.mul"] = 0;
  m_neededFiles["staidx3.mul"] = 0;
  m_neededFiles["statics3.mul"] = 0;
  
  m_neededFiles["map4.mul"] = 0;
  m_neededFiles["staidx4.mul"] = 0;
  m_neededFiles["statics4.mul"] = 0;
  
  m_neededFiles["map5.mul"] = 0;
  m_neededFiles["staidx5.mul"] = 0;
  m_neededFiles["statics5.mul"] = 0;
}

/**
 * @brief    Loads a map specified by number into the shared memory space in the client
 *
 * @param mapNumber Number corresponding to the map that the client should load (map0.mul, etc)
 */
void FileManager::LoadMap(uint8_t mapNumber)
{
  if (m_pMapFileStream->is_open())
  {
    m_pMapFileStream->close();
  }
  if (m_pStaidxFileStream->is_open())
  {
    m_pStaidxFileStream->close();
  }
  if (m_pStaticsFileStream->is_open())
  {
    m_pStaticsFileStream->close();
  }

  std::string filenameAndPath = BaseFileManager::getUltimaLiveSavePath();
  if (m_shardIdentifier != "")
  {
    filenameAndPath.append(m_shardIdentifier);
    filenameAndPath.append("\\");
  }

  std::string mapFileNameAndPath(filenameAndPath);
  char filename[32];
  sprintf_s(filename, "map%i.mul", mapNumber);
  mapFileNameAndPath.append(filename);

  std::string staidxFileNameAndPath(filenameAndPath);
  sprintf_s(filename, "staidx%i.mul", mapNumber);
  staidxFileNameAndPath.append(filename);

  std::string staticsFileNameAndPath(filenameAndPath);
  sprintf_s(filename, "statics%i.mul", mapNumber);
  staticsFileNameAndPath.append(filename);

  Logger::g_pLogger->LogPrint("Loading Map: %s\n", mapFileNameAndPath.c_str());

  std::ifstream mapFile;
  mapFile.open(mapFileNameAndPath, std::ios::binary | std::ios::in);
  if (mapFile.is_open())
  {
    mapFile.seekg (0, mapFile.end);
    std::streamoff length = mapFile.tellg();
    if (length > 0)
    {
      mapFile.seekg (0, mapFile.beg);
      mapFile.read(reinterpret_cast<char*>(m_pMapPool), length);
    }

    mapFile.close();
  }

  Logger::g_pLogger->LogPrint("Loading Staidx: %s\n", staidxFileNameAndPath.c_str());

  std::ifstream staidxFile;
  staidxFile.open(staidxFileNameAndPath, std::ios::binary | std::ios::in);

  if (staidxFile.is_open())
  {
    staidxFile.seekg (0, staidxFile.end);
    std::streamoff length = staidxFile.tellg();
    if (length > 0)
    {
      staidxFile.seekg (0, staidxFile.beg);
      staidxFile.read(reinterpret_cast<char*>(m_pStaidxPool), length);
    }
    staidxFile.close();
  }

  Logger::g_pLogger->LogPrint("Loading Statics: %s\n", staticsFileNameAndPath.c_str());

  std::ifstream staticsFile;
  staticsFile.open(staticsFileNameAndPath, std::ios::binary | std::ios::in);
  if (staticsFile.is_open())
  {
    staticsFile.seekg (0, staticsFile.end);
    std::streamoff length = staticsFile.tellg();
    if (length > 0)
    {
      staticsFile.seekg (0, staticsFile.beg);
      staticsFile.read(reinterpret_cast<char*>(m_pStaticsPool), length);
    }
    m_pStaticsPoolEnd = m_pStaticsPool + length;
    staticsFile.close();
  }

  m_pMapFileStream->open(mapFileNameAndPath, std::ios::out | std::ios::in | std::ios::binary);
  m_pStaidxFileStream->open(staidxFileNameAndPath, std::ios::out | std::ios::in | std::ios::binary);
  m_pStaticsFileStream->open(staticsFileNameAndPath, std::ios::out | std::ios::in | std::ios::binary);

  Logger::g_pLogger->LogPrint("Finished Loading Map!\n");
}

/**
 *
 * @brief Creates the shared memory spaces for map files, static files and indices
 *        This method is called in place of the client's original MapViewOfFile. Instead of mapping a view
 *        of each file that the client needs, a single shared space is mapped so that UltimaLive can dynamically
 *        load and unload maps as needed.
 * 
 * @param hFileMappingObject   A handle to a file mapping object.
 * @param dwDesiredAccess      The type of access to a file mapping object, which determines the protection of the pages.
 * @param dwFileOffsetHigh     A high-order DWORD of the file offset where the view begins.
 * @param dwFileOffsetLow      A low-order DWORD of the file offset where the view is to begin.
 * @param dwNumberOfBytesToMap The number of bytes of a file mapping to map to the view.
 *
 * @return The starting address of the mapped view
 */
LPVOID WINAPI FileManager::OnMapViewOfFile(
  __in  HANDLE hFileMappingObject,
  __in  DWORD dwDesiredAccess,
  __in  DWORD dwFileOffsetHigh,
  __in  DWORD dwFileOffsetLow,
  __in  SIZE_T dwNumberOfBytesToMap
  )
{
  ClientFileHandleSet* pMatchingFileset = NULL;

  for (std::map<std::string, ClientFileHandleSet*>::const_iterator iterator = m_files.begin(); iterator != m_files.end(); iterator++)
  {
    if (iterator->second->m_createFileMappingHandle == hFileMappingObject)
    {
      pMatchingFileset = iterator->second;
      break;
    }
  }

  HANDLE handleToReturn = INVALID_HANDLE_VALUE;

  if (pMatchingFileset != NULL)
  {
    std::string shortFilename = Utils::getFilenameFromPath(pMatchingFileset->m_filename);
    std::map<std::string, uint32_t>::iterator iterator = m_neededFiles.find(shortFilename);

    if (iterator != m_neededFiles.end())
    {
      if (shortFilename.find("map") != std::string::npos)
      {
          handleToReturn = m_pMapPool;
      }
      else if (shortFilename.find("statics") != std::string::npos)
      {
        handleToReturn = m_pStaticsPool;
      }
      else if (shortFilename.find("staidx") != std::string::npos)
      {
        handleToReturn = m_pStaidxPool;
      }
    }
  }

  if (handleToReturn == INVALID_HANDLE_VALUE)
  {
    handleToReturn = MapViewOfFile(hFileMappingObject, dwDesiredAccess, dwFileOffsetHigh, dwFileOffsetLow, dwNumberOfBytesToMap);
    if (pMatchingFileset != NULL)
    {
      pMatchingFileset->m_mapFileViewHandle = handleToReturn;
    }
  }

	return handleToReturn;
}

/**
 * @brief Finds a memory location corresponding to a block number
 *
 * @param mapNumber Map Number
 * @param blockNum Block Number to seek
 *
 * @return pointer to the block data 
 */
unsigned char* FileManager::seekLandBlock(uint8_t, uint32_t blockNum)
{
  uint8_t* pData = reinterpret_cast<unsigned char*>(m_pMapPool + (blockNum * 196) + 4);
  return pData;
}

/**
 * @brief Initialization function for the FileManager
 * 
 * @return True on success
 */
bool FileManager::Initialize()
{
  Logger::g_pLogger->LogPrint("Initializing MUL File Manager!\n");
  return BaseFileManager::Initialize();
}