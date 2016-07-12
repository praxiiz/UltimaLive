/* Copyright(c) 2016 UltimaLive
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

unsigned char* FileManager::seekLandBlock(uint8_t, uint32_t blockNum)
{
  uint8_t* pData = reinterpret_cast<unsigned char*>(m_pMapPool + (blockNum * 196) + 4);
  return pData;
}

unsigned char* FileManager::readLandBlock(uint8_t mapNumber, uint32_t blockNum)
{
  unsigned char* pBlockPosition = seekLandBlock(mapNumber, blockNum);

  unsigned char* pData = new uint8_t[192];

  if (pBlockPosition != NULL)
  {
    for (int i = 0; i < 192; ++i)
    {
      pData[i] = reinterpret_cast<unsigned char*>(pBlockPosition)[i];
    }
  }

  return pData;
}
 
bool FileManager::updateLandBlock(uint8_t mapNumber, uint32_t blockNum, uint8_t* pLandData)
{
  //update block in memory
  unsigned char* pBlockPosition = seekLandBlock(mapNumber, blockNum);

  Logger::g_pLogger->LogPrint("Land Block Memory Location: 0x%x\n", (int)pBlockPosition);


  if (pBlockPosition != NULL)
  {
    for (int i = 0; i < 192; ++i)
    {
      pBlockPosition[i] = pLandData[i];
    }
  }
  else
  {
    Logger::g_pLogger->LogPrint("Unable to update land block!\n");
  }

  if (m_pMapFileStream->is_open())
  {
    //update block on disk
    uint32_t blockSeekLocation = (blockNum * 196) + 4;
    Logger::g_pLogger->LogPrint("Updating Land block on disk at location: 0x%x\n", blockSeekLocation);
    m_pMapFileStream->seekp(blockSeekLocation, std::ios::beg);
  
#ifdef DEBUG
    if (m_pMapFileStream->bad())
    {
      Logger::g_pLogger->LogPrint("Failed to Seek!!!!!!!!!!!!!! (%i)\n", GetLastError());
    }
    else
    {
      Logger::g_pLogger->LogPrint("Seek successful\n");
    }
#endif
  
    m_pMapFileStream->write(const_cast<const char*>(reinterpret_cast<char*>(pLandData)), 192);
#ifdef DEBUG  
    if (m_pMapFileStream->bad())
    {
      Logger::g_pLogger->LogPrint("Failed to Write!!!!!!!!!!!!!! (%i)\n", GetLastError());
    }
    else
    {
      Logger::g_pLogger->LogPrint("Wrote successfully\n");
    }
#endif
  }

  m_pMapFileStream->flush();
  Logger::g_pLogger->LogPrint("Flushed successfully\n");

  return true;
}

bool FileManager::Initialize()
{
  Logger::g_pLogger->LogPrint("Initializing MUL File Manager!\n");
  return BaseFileManager::Initialize();
}