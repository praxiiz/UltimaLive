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

#include "BaseFileManager.h"

#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#include "shlobj.h"
#include "..\Maps\MapDefinition.h"

/** 
 * @brief Reads a land block and returns a pointer to the newly allocated memory. Caller is responsible for memory cleanup.
 *
 * @param mapNumber number corresponding to the map to search
 * @param blockNum number corresponding to the block in the map
 *
 * @return Pointer to the block memory
 */
unsigned char* BaseFileManager::readLandBlock(uint8_t mapNumber, uint32_t blockNum)
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

/** 
 * @brief updates a land block in memory and in UltimaLive's map cache on disk
 *
 * @param mapNumber number corresponding to the map (e.g. map0.mul)
 * @param blockNum number corresponding to the map block to be updated
 * @param pLandData pointer to new data to replace the existing land data
 *
 * @return True on success
 */
bool BaseFileManager::updateLandBlock(uint8_t mapNumber, uint32_t blockNum, uint8_t* pLandData)
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

/** 
 * @brief reads data corresponding to all statics contained in one map block
 *
 * @param mapNumber Map Number
 * @param blockNum Block Number
 * @param rNumberOfBytesOut number of bytes read
 * 
 * @return pointer to memory containing statics in the block
 */
unsigned char* BaseFileManager::readStaticsBlock(uint32_t, uint32_t blockNum, uint32_t& rNumberOfBytesOut)
{
  uint8_t* pBlockIdx = m_pStaidxPool;
  pBlockIdx += (blockNum * 12);

  uint32_t lookup = *reinterpret_cast<uint32_t*>(pBlockIdx); 
  rNumberOfBytesOut = *reinterpret_cast<uint32_t*>(pBlockIdx + 4); //length
  uint8_t* pRawStaticData = NULL;

  if (lookup >= 0 && lookup < STATICS_MEMORY_SIZE && rNumberOfBytesOut > 0 && rNumberOfBytesOut != 0xFFFFFFFF)
  {
    uint8_t* pStatics = reinterpret_cast<uint8_t*>(m_pStaticsPool);
    pStatics += lookup;

    pRawStaticData = new uint8_t[rNumberOfBytesOut];

    for(uint32_t i = 0; i < rNumberOfBytesOut; ++i)
    {
      pRawStaticData[i] = pStatics[i];
    }
  }

  return pRawStaticData;
}

/** 
 * @brief Writes data corresponding to the statics in a map block out to memory and to the UltimaLive file cache
 *
 * @param mapNumber Map Number
 * @param blockNum Block number
 * @param pBlockData Pointer tot he new block data
 * @param updatedStaticsLength Number of bytes in the new data
 *
 * @return true on success
 */
bool BaseFileManager::writeStaticsBlock(uint8_t, uint32_t blockNum, uint8_t* pBlockData, uint32_t updatedStaticsLength)
{
  Logger::g_pLogger->LogPrint("Writing statics: %i\n", blockNum);

  char buffer[sizeof(uint32_t)];
  uint8_t* pBlockIdx = m_pStaidxPool;
  pBlockIdx += (blockNum * 12);

  //Zero length statics block is a corner case
  if (updatedStaticsLength <= 0)
  {
    Logger::g_pLogger->LogPrint("writing block with zero statics\n");

    //update index length in memory
    *reinterpret_cast<uint32_t*>(pBlockIdx + 4) = 0;
    
    //update index lookup in memory
    *reinterpret_cast<uint32_t*>(pBlockIdx) = 0xFFFFFFFF;

    //update index length on disk
    *reinterpret_cast<uint32_t*>(buffer) = 0;
    m_pStaidxFileStream->seekp((blockNum * 12) + 4, std::ios::beg);
    m_pStaidxFileStream->write(buffer, sizeof(uint32_t));

    //update index lookup on disk
    *reinterpret_cast<uint32_t*>(buffer) = 0xFFFFFFFF;
    m_pStaidxFileStream->seekp(blockNum * 12, std::ios::beg);
    m_pStaidxFileStream->write(buffer, sizeof(uint32_t));
    m_pStaidxFileStream->flush();

    return true;
  }

  uint32_t existingLookup = *reinterpret_cast<uint32_t*>(pBlockIdx); 
  Logger::g_pLogger->LogPrint("Existing lookup: 0x%x\n", existingLookup);
  uint32_t existingStaticsLength = *reinterpret_cast<uint32_t*>(pBlockIdx + 4);

  //update index length in memory
  *reinterpret_cast<uint32_t*>(pBlockIdx + 4) = updatedStaticsLength;

  //update index length on disk
  *reinterpret_cast<uint32_t*>(buffer) = updatedStaticsLength;
  m_pStaidxFileStream->seekp((blockNum * 12) + 4, std::ios::beg);
  m_pStaidxFileStream->write(buffer, sizeof(uint32_t));

  //Do we have enough room to write the statics into the existing location?
  if (existingStaticsLength >= updatedStaticsLength && existingLookup != 0xFFFFFFFF && existingLookup < STATICS_MEMORY_SIZE)
  {
    Logger::g_pLogger->LogPrint("writing statics to existing file location at 0x%x, length:%i\n", existingLookup, updatedStaticsLength);

    //update memory
    uint8_t* pStatics = m_pStaticsPool;
    pStatics += existingLookup;
    memcpy(pStatics, pBlockData, updatedStaticsLength);

    //update disk
    m_pStaticsFileStream->seekp(existingLookup, std::ios::beg);
    m_pStaticsFileStream->write((const char*)pBlockData, updatedStaticsLength);
  }
  else
  {
    uint32_t newLookup = m_pStaticsPoolEnd - m_pStaticsPool;
    Logger::g_pLogger->LogPrint("writing statics to end of file 0x%x, length:%i\n", newLookup, updatedStaticsLength);

    //update index lookup in memory
    *reinterpret_cast<uint32_t*>(pBlockIdx) = newLookup;

    //update index lookup on disk
    *reinterpret_cast<uint32_t*>(buffer) = newLookup;
    m_pStaidxFileStream->seekp(blockNum * 12, std::ios::beg);
    m_pStaidxFileStream->write(buffer, sizeof(uint32_t));

    //update statics in memory
    memcpy(m_pStaticsPoolEnd, pBlockData, updatedStaticsLength);

    //update statics on disk
    m_pStaticsFileStream->seekp(newLookup, std::ios::beg);
    m_pStaticsFileStream->write((const char*)pBlockData, updatedStaticsLength);

    m_pStaticsPoolEnd += updatedStaticsLength;
  }

  m_pStaidxFileStream->flush();
  m_pStaticsFileStream->flush();
  return true;
}

/** 
 * @brief Initializes the BaseFileManager by allocating memory internally in the client for maps, statics, and indices
 * 
 * @return true on success
 */
bool BaseFileManager::Initialize()
{
  CreateDirectoryA(getUltimaLiveSavePath().c_str(), NULL);

  bool success = true;

  Logger::g_pLogger->LogPrintWithoutDate("   Allocating %-12i bytes for client map handling                 ", MAP_MEMORY_SIZE);
  m_pMapPool = reinterpret_cast<uint8_t*>(VirtualAlloc(NULL, MAP_MEMORY_SIZE, MEM_COMMIT, PAGE_EXECUTE_READWRITE));
  Logger::g_pLogger->LogPrintWithoutDate(" [0x%08x] ", m_pMapPool);
  checkValidMemoryAllocated(m_pMapPool);

  if (success)
  {
    Logger::g_pLogger->LogPrintWithoutDate("   Allocating %-12i bytes for client statics index handling       ", STAIDX_MEMORY_SIZE);
    m_pStaidxPool = reinterpret_cast<uint8_t*>(VirtualAlloc(NULL, STAIDX_MEMORY_SIZE, MEM_COMMIT, PAGE_EXECUTE_READWRITE));
    Logger::g_pLogger->LogPrintWithoutDate(" [0x%08x] ", m_pStaidxPool);
    checkValidMemoryAllocated(m_pStaidxPool);
  }

  if (success)
  {
    Logger::g_pLogger->LogPrintWithoutDate("   Allocating %-12i bytes for client statics handling             ", STATICS_MEMORY_SIZE);
    m_pStaticsPool = reinterpret_cast<uint8_t*>(VirtualAlloc(NULL, STATICS_MEMORY_SIZE, MEM_COMMIT, PAGE_EXECUTE_READWRITE));
    m_pStaticsPoolEnd = m_pStaticsPool;
    Logger::g_pLogger->LogPrintWithoutDate(" [0x%08x] ", m_pStaticsPool);
    checkValidMemoryAllocated(m_pStaidxPool);
  }

  return success;
}

/** 
 * @brief handles client logout and prepares UltimaLive for a new login
 */
void BaseFileManager::onLogout()
{
  Logger::g_pLogger->LogPrint("Closing map, staidx, statics file streams\n");

  if (m_pMapFileStream != NULL)
  {
    m_pMapFileStream->flush();
    m_pMapFileStream->close();
  }

  if (m_pStaidxFileStream != NULL)
  {
    m_pStaidxFileStream->flush();
    m_pStaidxFileStream->close();
  }

  if (m_pStaticsFileStream != NULL)
  {
    m_pStaticsFileStream->flush();
    m_pStaticsFileStream->close();
  }
}

/** 
 * @brief Creates a new blank map in the map store from scratch
 * 
 * @param pathWithoutFilename location to save file
 * @param mapNumber  Number to be appended to the new filename
 * @param numHorizontalBlocks Number of horizontal blocks for the new map
 * @param numVerticalBlocks Number of vertical blocks for the new map
 *
 * @return true on success
 */
bool BaseFileManager::createNewPersistentMap(std::string pathWithoutFilename, uint8_t mapNumber, uint32_t numHorizontalBlocks, uint32_t numVerticalBlocks)
{
  //create file names
  std::string mapPath(pathWithoutFilename);
  mapPath.append("\\");
  std::string staidxPath(mapPath);
  std::string staticsPath(mapPath);
  char filename[32];
  sprintf_s(filename, "map%i.mul", mapNumber);
  mapPath.append(filename);
  sprintf_s(filename, "staidx%i.mul", mapNumber);
  staidxPath.append(filename);
  sprintf_s(filename, "statics%i.mul", mapNumber);
  staticsPath.append(filename);

  //create map new file
  std::fstream mapFile;
  mapFile.open(mapPath, std::ios::out | std::ios::app | std::ios::binary | std::ios::in);

  Logger::g_pLogger->LogPrint("Creating map file\n");
  Logger::g_pLogger->LogPrint("Writing %u blocks by %u blocks\n", numHorizontalBlocks, numVerticalBlocks);

  uint32_t numberOfBytesInStrip = 196 * numVerticalBlocks;
  char* pVerticalBlockStrip = new char[numberOfBytesInStrip];

  const char block[196] = {
                          0x00, 0x00, 0x00, 0x00, //header
                          0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00,
                          0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00,
                          0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00,
                          0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00,
                          0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00,
                          0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00,
                          0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00,
                          0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00, 0x44, 0x02, 0x00 };
  for (uint32_t y = 0; y < numVerticalBlocks; y++)
  {
    memcpy(&pVerticalBlockStrip[196 * y], block, 196);
  }  

  for (uint32_t x = 0; x < numHorizontalBlocks; x++)
  {
    mapFile.write(pVerticalBlockStrip, numberOfBytesInStrip);
  }
  delete [] pVerticalBlockStrip;
  mapFile.close();

  std::fstream staidxFile;
  staidxFile.open(staidxPath, std::ios::out | std::ios::app | std::ios::binary | std::ios::in);

  Logger::g_pLogger->LogPrint("Creating index file\n");
  Logger::g_pLogger->LogPrint("W %u blocks by %u blocks\n", numHorizontalBlocks, numVerticalBlocks);

  uint32_t numberOfBytesInIndexStrip = 12 * numVerticalBlocks;
  char* pVerticalBlockIndexStrip = new char[numberOfBytesInIndexStrip];
  unsigned char blankIndex[12] = { 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

  for (uint32_t y = 0; y < numVerticalBlocks; y++)
  {
    memcpy(&pVerticalBlockIndexStrip[12 * y], blankIndex, 12);
  }  

  for (uint32_t x = 0; x < numHorizontalBlocks; x++)
  {
    staidxFile.write(pVerticalBlockIndexStrip, numberOfBytesInIndexStrip);
  }
  delete [] pVerticalBlockIndexStrip;
  staidxFile.close();

  std::fstream staticsFile;
  staticsFile.open(staticsPath, std::ios::out | std::ios::app | std::ios::binary | std::ios::in);
  staticsFile.close();

  return true;
}

/**
 * @brief Copy a file from one location to another
 * 
 * @param sourceFilePath source path
 * @param destFilePath destination path
 * @param pProgress progress bar handle
 */
void BaseFileManager::copyFile(std::string sourceFilePath, std::string destFilePath, ProgressBarDialog* pProgress)
{
  std::ifstream sourceFile(sourceFilePath, std::ifstream::binary);

  if (!sourceFile.good())
  {
    sourceFile.close();
    return;
  }

  sourceFile.seekg(0, sourceFile.end);
  std::streamoff totalFileSizeInBytes = sourceFile.tellg();
  sourceFile.close();

  uint32_t bytesCopied = 0;
  uint32_t prevPercent = 0;

  // allocate memory for buffer
  char* pBuffer = new char[4096];
  size_t bytesToWrite;

  FILE* pSource = NULL;
  FILE* pDest = NULL;

  errno_t err = fopen_s(&pSource, sourceFilePath.c_str(), "rb");
  err = fopen_s(&pDest, destFilePath.c_str(), "wb");

  // clean and more secure
  // feof(FILE* stream) returns non-zero if the end of file indicator for stream is set
  do 
  {
    bytesToWrite = fread(pBuffer, 1, 4096, pSource);
    if (bytesToWrite != 0)
    {
      fwrite(pBuffer, 1, bytesToWrite, pDest);
      bytesCopied += bytesToWrite;
      
      if (pProgress != NULL)
      {
        uint32_t percent = (uint32_t)(((float)bytesCopied / (float)totalFileSizeInBytes) * 100.0f);
        if ((uint32_t)percent > prevPercent)
        {
          prevPercent = percent;
          pProgress->setProgress((uint32_t)percent);
        }
      }
    }
  } while (bytesToWrite != 0);

  delete [] pBuffer;

  fclose(pSource);
  fclose(pDest);
}

/**
 * @brief Initializes a shards maps by creating or copying new maps into the UltimaLive file cache
 * 
 * @param shardIdentifier Unique shard identifier
 * @param mapDefinitions Map definitions for the shard
 */
void BaseFileManager::InitializeShardMaps(std::string shardIdentifier, std::map<uint32_t, MapDefinition> mapDefinitions)
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
      //copy existing maps if they match the dimensions specified in the map definitions
      uint32_t fileSizeNeeded = (itr->second.mapWidthInTiles >> 3) * (itr->second.mapHeightInTiles >> 3) * 196;

      Logger::g_pLogger->LogPrint("File Needs to be %u bytes\n", fileSizeNeeded);
      std::string clientFolder = Utils::GetCurrentPathWithoutFilename();;
      std::string existingFilePath(clientFolder);
      existingFilePath.append("\\");
      existingFilePath.append(filename);

      std::streamoff currentFileSize = 0;

      //get file size
      std::ifstream existingMapFile(existingFilePath, std::ifstream::binary);
      if (existingMapFile.good())
      {
        existingMapFile.seekg(0, existingMapFile.end);
        currentFileSize = existingMapFile.tellg();
      }
      existingMapFile.close();

      Logger::g_pLogger->LogPrint("%s\n", existingFilePath.c_str());
      Logger::g_pLogger->LogPrint("Existing file size: %u\n", currentFileSize);

      if (fileSizeNeeded == currentFileSize)
      {
        Logger::g_pLogger->LogPrint("Copying File: %s to %s ... ", existingFilePath.c_str(), filePath.c_str());

        //map file
        std::string mapMessage("Copying ");
        mapMessage.append(filename);
        mapMessage.append(" from game client folder");
        m_pProgressDlg->setMessage(mapMessage);
        m_pProgressDlg->setProgress(0);
        copyFile(existingFilePath, filePath, m_pProgressDlg);

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

/**
 * @brief Gets the UltimaLive cache path for the platform
 *
 * @return UltimaLive cache path
 */
std::string BaseFileManager::getUltimaLiveSavePath()
{
  char szPath[MAX_PATH];
  // Get path for each computer, non-user specific and non-roaming data.
  if ( SUCCEEDED( SHGetFolderPath( NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath ) ) )
  {
      // Append product-specific path
      PathAppend( szPath, "\\UltimaLive\\");
  }
  return std::string(szPath);
}

/**
 * @brief BaseFileManager constructor
 */
BaseFileManager::BaseFileManager()
  : m_files(),
  m_pMapPool(NULL),
  m_pStaticsPool(NULL),
  m_pStaticsPoolEnd(NULL),
  m_pStaidxPool(NULL),
  m_pStaidxPoolEnd(NULL),
  m_shardIdentifier(""),
  m_pMapFileStream(new std::ofstream()),
  m_pStaidxFileStream(new std::ofstream()),
  m_pStaticsFileStream(new std::ofstream()),
  m_pProgressDlg()
{
  //do nothing
}

/**
 * @brief Called when the client calls its CloseHandle function. Removes filenames from the list of open handles.
 * 
 * @param hObject Object Handle
 * 
 * @return true on success
 */
BOOL WINAPI BaseFileManager::OnCloseHandle(_In_ HANDLE hObject)
{
  for (std::map<std::string, ClientFileHandleSet*>::const_iterator iterator = m_files.begin(); iterator != m_files.end(); iterator++)
  {
    if (iterator->second->m_createFileHandle == hObject)
    {
        m_files.erase(iterator);
        break;
    }
  }

  return true;
}

/**
 * @brief Called when the client calls CreateFileA. Caches object handles needed by filemanagers. 
 *
 * @param lpFileName             The name of the file or device to be created or opened.
 * @param dwDesiredAccess        The requested access to the file or device, which can be summarized as read, write, both or neither.
 * @param dwShareMode            The requested sharing mode of the file or device, which can be read, write, both, delete, all of these, or none.
 * @param lpSecurityAttributes   A pointer to a SECURITY_ATTRIBUTES structure that contains two separate but related data members.
 * @param dwCreationDisposition  An action to take on a file or device that exists or does not exist.
 * @param dwFlagsAndAttributes   The file or device attributes and flags, FILE_ATTRIBUTE_NORMAL being the most common default value for files.
 * @param hTemplateFile          A valid handle to a template file with the GENERIC_READ access right.
 *
 * @return Object Handle
 */
HANDLE WINAPI BaseFileManager::OnCreateFileA(
  __in      LPCSTR lpFileName,
  __in      DWORD dwDesiredAccess,
  __in      DWORD dwShareMode,
  __in_opt  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
  __in      DWORD dwCreationDisposition,
  __in      DWORD dwFlagsAndAttributes,
  __in_opt  HANDLE hTemplateFile
  )
{
  std::string originalFilename(lpFileName);
  std::string filename = Utils::getFilenameFromPath(originalFilename);

  std::map<std::string, ClientFileHandleSet*>::iterator pMatchingFileSetItr = m_files.find(filename);

  HANDLE handleToBeReturned = CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

  if (pMatchingFileSetItr == m_files.end())
  {
    m_files[filename] = new ClientFileHandleSet(originalFilename);
    m_files[filename]->m_createFileHandle = handleToBeReturned;
  }

  return handleToBeReturned;
}

/**
 * @brief Called when the client calls createfilemapping. Stores a file mapping with a matching fileset if its a file UltimaLive Tracks.
 *
 * @param hFile              A handle to the file from which to create a file mapping object.
 * @param lpAttributes       A pointer to a SECURITY_ATTRIBUTES structure that determines whether a returned handle can be inherited by child processes.
 * @param flProtect          Specifies the page protection of the file mapping object.
 * @param dwMaximumSizeHigh  The high-order DWORD of the maximum size of the file mapping object.
 * @param dwMaximumSizeLow   The low-order DWORD of the maximum size of the file mapping object.
 * @param lpName             The name of the file mapping object.
 *
 * @return New File's Handle
 */
HANDLE WINAPI BaseFileManager::OnCreateFileMappingA(
  __in      HANDLE hFile,
  __in_opt  LPSECURITY_ATTRIBUTES lpAttributes,
  __in      DWORD flProtect,
  __in      DWORD dwMaximumSizeHigh,
  __in      DWORD dwMaximumSizeLow,
  __in_opt  LPCSTR lpName
  )
{
  ClientFileHandleSet* pMatchingFileset = NULL;

  for (std::map<std::string, ClientFileHandleSet*>::const_iterator iterator = m_files.begin(); iterator != m_files.end(); iterator++)
  {
    if (hFile == iterator->second->m_createFileHandle)
    {
      pMatchingFileset = iterator->second;
      break;
    }
  }
  
  HANDLE handleToReturn = CreateFileMappingA(hFile, lpAttributes, flProtect, dwMaximumSizeHigh, dwMaximumSizeLow, lpName);

  if (pMatchingFileset != NULL)
  {
    pMatchingFileset->m_createFileMappingHandle = handleToReturn;
  }

	return handleToReturn;
}

/**
 * @brief Called in place of client's MapViewOfFile. Stores mapping address if it corresponds to a file that UltimaLive uses.
 *
 * @param hFileMappingObject     A handle to a file mapping object.
 * @param dwDesiredAccess        The type of access to a file mapping object, which determines the protection of the pages.
 * @param dwFileOffsetHigh       A high-order DWORD of the file offset where the view begins.
 * @param dwFileOffsetLow        A low-order DWORD of the file offset where the view is to begin.
 * @param dwNumberOfBytesToMap   The number of bytes of a file mapping to map to the view.
 *
 * @return The starting address of the mapped view
 */
LPVOID WINAPI BaseFileManager::OnMapViewOfFile(
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
    if (hFileMappingObject == iterator->second->m_createFileMappingHandle)
    {
      pMatchingFileset = iterator->second;
      break;
    }
  }

  HANDLE handleToReturn = handleToReturn = MapViewOfFile(hFileMappingObject, dwDesiredAccess, dwFileOffsetHigh, dwFileOffsetLow, dwNumberOfBytesToMap); 

  if (pMatchingFileset != NULL)
  {
    pMatchingFileset->m_mapFileViewHandle = handleToReturn;
  }

	return handleToReturn;
}

/**
 * @brief Checks to see if UltimaLive has allocated a valid pool of memory
 *
 * @return true on success
 */
bool BaseFileManager::checkValidMemoryAllocated(void*)
{
  bool success = true;
  if (m_pMapPool != NULL)
  {
    Logger::g_pLogger->LogPrintTaskStatusResult(true);
  }
  else
  {
    success = false;
    Logger::g_pLogger->LogPrintTaskStatusResult(false);
    Logger::g_pLogger->LogPrintError("Failed to allocate enough memory.\n");
    Logger::g_pLogger->LogLastErrorMessage();
  }
  return success;
}
