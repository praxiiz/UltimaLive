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

#include "FileManager_7_0_29_2.h"
#include <cstdio>
#include "..\Uop\UopUtility.h"
#include "..\..\Maps\MapDefinition.h"

FileManager_7_0_29_2::FileManager_7_0_29_2()
  : BaseFileManager(),
    m_fileEntries(),
    m_neededFiles()
{
  m_neededFiles["map0LegacyMUL.uop"] = 0;
  m_neededFiles["staidx0.mul"] = 0;
  m_neededFiles["statics0.mul"] = 0;

  m_neededFiles["map1LegacyMUL.uop"] = 0;
  m_neededFiles["staidx1.mul"] = 0;
  m_neededFiles["statics1.mul"] = 0;
  
  m_neededFiles["map2LegacyMUL.uop"] = 0;
  m_neededFiles["staidx2.mul"] = 0;
  m_neededFiles["statics2.mul"] = 0;
  
  m_neededFiles["map3LegacyMUL.uop"] = 0;
  m_neededFiles["staidx3.mul"] = 0;
  m_neededFiles["statics3.mul"] = 0;
  
  m_neededFiles["map4LegacyMUL.uop"] = 0;
  m_neededFiles["staidx4.mul"] = 0;
  m_neededFiles["statics4.mul"] = 0;
  
  m_neededFiles["map5LegacyMUL.uop"] = 0;
  m_neededFiles["staidx5.mul"] = 0;
  m_neededFiles["statics5.mul"] = 0;

  //handle loading in the base class
}

void FileManager_7_0_29_2::LoadMap(uint8_t mapNumber)
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

  Logger::g_pLogger->LogPrint("******************Loading Map: %s *************************\n", mapFileNameAndPath.c_str());

  std::ifstream mapFile;
  mapFile.open(mapFileNameAndPath, std::ios::binary | std::ios::in);
  if (mapFile.is_open())
  {
    int numFilesInMap = m_fileEntries.size();
    for (int i = 0; i < numFilesInMap; i++)
    {
      FileEntry* pCurrentEntry = m_fileEntries[i];
      char* offset = reinterpret_cast<char*>(m_pMapPool + pCurrentEntry->MetaDataSize + pCurrentEntry->UopFileOffset);
      mapFile.read(offset, pCurrentEntry->UncompressedDataSize);
    }
    mapFile.close();
  }

  Logger::g_pLogger->LogPrint("******************Loading Staidx: %s *************************\n", staidxFileNameAndPath.c_str());

  std::ifstream staidxFile;
  staidxFile.open(staidxFileNameAndPath, std::ios::binary | std::ios::in);

  if (staidxFile.is_open())
  {
    staidxFile.seekg (0, staidxFile.end);
    std::streamoff length = staidxFile.tellg();
    staidxFile.seekg (0, staidxFile.beg);
    staidxFile.read(reinterpret_cast<char*>(m_pStaidxPool), length);
    staidxFile.close();
  }

  Logger::g_pLogger->LogPrint("******************Loading Statics: %s *************************\n", staticsFileNameAndPath.c_str());

  std::ifstream staticsFile;
  staticsFile.open(staticsFileNameAndPath, std::ios::binary | std::ios::in);
  if (staticsFile.is_open())
  {
    staticsFile.seekg (0, staticsFile.end);
    std::streamoff length = staticsFile.tellg();
    staticsFile.seekg (0, staticsFile.beg);
    staticsFile.read(reinterpret_cast<char*>(m_pStaticsPool), length);
    m_pStaticsPoolEnd = m_pStaticsPool + length;
    staticsFile.close();
  }

  m_pMapFileStream->open(mapFileNameAndPath, std::ios::out | std::ios::in | std::ios::binary);
  m_pStaidxFileStream->open(staidxFileNameAndPath, std::ios::out | std::ios::in | std::ios::binary);
  m_pStaticsFileStream->open(staticsFileNameAndPath, std::ios::out | std::ios::in | std::ios::binary);

  Logger::g_pLogger->LogPrint("##################   Finished Loading Map!\n");
}

bool FileManager_7_0_29_2::Initialize()
{
  Logger::g_pLogger->LogPrint("Initializing UOP File Manager!\n");
  return BaseFileManager::Initialize();
}

LPVOID WINAPI FileManager_7_0_29_2::OnMapViewOfFile(
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

        if (shortFilename == "map0LegacyMUL.uop")
        {
          std::ifstream map0(m_files["map0LegacyMUL.uop"]->m_filename, std::ios::in|std::ios::binary);

          if (map0.is_open())
          {
            map0.seekg (0, map0.end);
            std::streamoff length = map0.tellg();
            map0.seekg (0, map0.beg);
            map0.read(reinterpret_cast<char*>(m_pMapPool), length);
            map0.close();
          }
          else
          {
            Logger::g_pLogger->LogPrint("FAILED TO OPEN FILE\n");
          }
          parseMapFile("map0legacymul");
        }
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

void FileManager_7_0_29_2::parseMapFile(std::string filename)
{
  UopHeader header; 
  header.unmarshal(m_pMapPool);
  
  std::map<uint32_t, uint64_t>* pHashes = UopUtility::getMapHashes(header.TotalFiles, filename);
  
  //unmarshal file table
  FileTable table;
  table.unmarshal(m_pMapPool + header.FileTableOffset);
  
  //make a list of file entries and map them in order
  for (uint32_t i = 0; i < header.TotalFiles; ++i)
  {
    //find the corresponding FileEntry
    for (uint32_t j = 0; j < header.TotalFiles; ++j)
    {
      if ((*pHashes)[i] == table.pEntries[j].PathChecksum)
      {
        m_fileEntries[i] = new FileEntry(table.pEntries[j]);
        break;
      }
    }
  }

  delete pHashes;
}

unsigned char* FileManager_7_0_29_2::seekLandBlock(uint8_t, uint32_t blockNum)
{
  uint8_t* pData = NULL;
  uint32_t blockSeekLocation = blockNum * 196;
  int numFilesInMap = m_fileEntries.size();
  uint32_t fileSeekLocation = 0;
    
  for (int i = 0; i < numFilesInMap; i++)
  {
    FileEntry* pCurrentEntry = m_fileEntries[i];
    
    if (blockSeekLocation < (fileSeekLocation + pCurrentEntry->UncompressedDataSize))
    {
      uint32_t blockOffset = blockSeekLocation - fileSeekLocation;
      uint32_t fileViewAddress = (uint32_t)m_pMapPool;
      uint32_t fileEntryAddress = static_cast<uint32_t>(fileViewAddress + pCurrentEntry->UopFileOffset);
      pData = reinterpret_cast<unsigned char*>(fileEntryAddress + blockOffset + pCurrentEntry->MetaDataSize + 4);
    
      break;
    }
    
    fileSeekLocation += pCurrentEntry->UncompressedDataSize;
  }

  return pData;
}

unsigned char* FileManager_7_0_29_2::readLandBlock(uint8_t mapNumber, uint32_t blockNum)
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
 
bool FileManager_7_0_29_2::updateLandBlock(uint8_t mapNumber, uint32_t blockNum, uint8_t* pLandData)
{
  //update block in memory
  unsigned char* pBlockPosition = seekLandBlock(mapNumber, blockNum);
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

void FileManager_7_0_29_2::InitializeShardMaps(std::string shardIdentifier, std::map<uint32_t, MapDefinition> mapDefinitions)
{
  m_pProgressDlg = new ProgressBarDialog();
  m_pProgressDlg->show();

  Logger::g_pLogger->LogPrint("(((((((((((((Initializing Shard Maps)))))))))))))))))\n");

  m_shardIdentifier = shardIdentifier;

  std::string shardFullPath(getUltimaLiveSavePath());
  shardFullPath.append("\\");
  shardFullPath.append(shardIdentifier);
  CreateDirectoryA(shardFullPath.c_str(), NULL);

  for (std::map<uint32_t, MapDefinition>::iterator itr = mapDefinitions.begin(); itr != mapDefinitions.end(); itr++)
  {
    std::string filePath(shardFullPath);
    filePath.append("\\");
    char filename[32];
    sprintf_s(filename, "map%i.mul", itr->first);
    filePath.append(filename);

    Logger::g_pLogger->LogPrint("Checking for %s\n", filePath.c_str());

    std::fstream mapFile(filePath);

    if (mapFile.good())
    {
      Logger::g_pLogger->LogPrint("File exists and is ok\n");
    }
    else
    {
      //Convert from existing map#LegacyMUL.uop files to map#.mul
      uint32_t fileSizeNeeded = (itr->second.mapWidthInTiles >> 3) * (itr->second.mapHeightInTiles >> 3) * 196;
      uint32_t blocksNeeded = fileSizeNeeded / 196;

      Logger::g_pLogger->LogPrint("Map Dimensions specify %u bytes\n", fileSizeNeeded);
      Logger::g_pLogger->LogPrint("Map Dimensions specify %u blocks\n", blocksNeeded);

      //get existing map#LegacyMUL.uop fully qualified path
      std::string clientFolder = Utils::GetCurrentPathWithoutFilename();;
      std::string existingFilePath(clientFolder);
      existingFilePath.append("\\");
      char uopFilename[32];
      sprintf_s(uopFilename, "map%iLegacyMUL.uop", itr->first);
      existingFilePath.append(uopFilename);

      uint32_t currentFileSize = UopUtility::getUopMapSizeInBytes(existingFilePath);
      uint32_t currentFileBlocks = currentFileSize / 196;
      Logger::g_pLogger->LogPrint("Existing file is %u bytes\n", currentFileSize);
      Logger::g_pLogger->LogPrint("Existing file has %u blocks\n", currentFileBlocks);

      if (currentFileBlocks <= blocksNeeded + 1 && currentFileBlocks >= blocksNeeded - 1)
      {
        Logger::g_pLogger->LogPrint("Copying File: %s to %s ... ", existingFilePath.c_str(), filePath.c_str());

        //map file
        std::string mapMessage("Importing ");
        mapMessage.append(filename);
        mapMessage.append(" from game client folder");
        m_pProgressDlg->setMessage(mapMessage);
        m_pProgressDlg->setProgress(0);
        UopUtility::convertUopMapToMul(existingFilePath, filePath, m_pProgressDlg);

        Logger::g_pLogger->LogPrint("done!\n");

        //statics file
        std::string dstStaticsFilePath(shardFullPath);
        dstStaticsFilePath.append("\\");
        char staticsFilename[32];
        sprintf_s(staticsFilename, "statics%i.mul", itr->first);
        dstStaticsFilePath.append(staticsFilename);
        std::string staticsFilePath (clientFolder);
        staticsFilePath.append("\\");
        staticsFilePath.append(staticsFilename);

        Logger::g_pLogger->LogPrint("Copying File: %s to %s ... ", staticsFilePath.c_str(), dstStaticsFilePath.c_str());

        std::string staticsMessage("Copying ");
        staticsMessage.append(staticsFilename);
        staticsMessage.append(" from game client folder");
        m_pProgressDlg->setMessage(staticsMessage);
        m_pProgressDlg->setProgress(0);
        copyFile(staticsFilePath, dstStaticsFilePath, m_pProgressDlg);

        Logger::g_pLogger->LogPrint("done!\n");

        //statics idx
        std::string dstStaidxFilePath(shardFullPath);
        dstStaidxFilePath.append("\\");
        char staidxFilename[32];
        sprintf_s(staidxFilename, "staidx%i.mul", itr->first);
        dstStaidxFilePath.append(staidxFilename);
        std::string staidxFilePath (clientFolder);
        staidxFilePath.append("\\");
        staidxFilePath.append(staidxFilename);

        Logger::g_pLogger->LogPrint("Copying File: %s to %s ... ", staidxFilePath.c_str(), dstStaidxFilePath.c_str());

        std::string staidxMessage("Copying ");
        staidxMessage.append(filename);
        staidxMessage.append(" from game client folder");
        m_pProgressDlg->setMessage(staidxMessage);
        m_pProgressDlg->setProgress(0);
        copyFile(staidxFilePath, dstStaidxFilePath, m_pProgressDlg);

        Logger::g_pLogger->LogPrint("done!\n");
      }
      else
      {
        createNewPersistentMap(shardFullPath, static_cast<uint8_t>(itr->first), itr->second.mapWidthInTiles >> 3, itr->second.mapWrapHeightInTiles >> 3);
      }
    }

    mapFile.close();
  }


  m_pProgressDlg->hide();
  delete m_pProgressDlg;
  m_pProgressDlg = NULL;
}