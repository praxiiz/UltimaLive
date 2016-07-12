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

#include "Atlas.h"
#include "..\UltimaLive.h"
#include "..\Network\NetworkManager.h"
#include "..\FileSystem\BaseFileManager.h"

#pragma region Self Registration
SelfRegisteringClass <Atlas> Atlas::m_registration;


void Atlas::Configure()
{
  Logger::g_pLogger->LogPrint("Atlas configure\n");
  UltimaLive::g_pUltimaLive->Register("Atlas", new Atlas());
}

bool Atlas::Initialize()
{
  bool success = true;

  Logger::g_pLogger->LogPrint("Initializing Atlas\n");
  Atlas* pInstance = static_cast<Atlas*>(UltimaLive::g_pUltimaLive->Lookup("Atlas"));

  if (pInstance != NULL)
  {
    BaseFileManager* pFileManager = static_cast<BaseFileManager*>(UltimaLive::g_pUltimaLive->Lookup("FileManager"));
    if (pFileManager == NULL)
    {
      Logger::g_pLogger->LogPrintError("Atlas: Failed to acquire File Manager Instance\n");
      success = false;
    }

    Client* pClient = static_cast<Client*>(UltimaLive::g_pUltimaLive->Lookup("Client"));
    if (pClient == NULL)
    {
      Logger::g_pLogger->LogPrintError("Atlas: Failed to acquire Client class Instance\n");
      success = false;
    }

    NetworkManager* pNetworkManager = static_cast<NetworkManager*>(UltimaLive::g_pUltimaLive->Lookup("NetworkManager"));
    if (pNetworkManager == NULL)
    {
      Logger::g_pLogger->LogPrintError("Atlas: Failed to acquire Network Manager Instance\n");
      success = false;
    }
    else
    {
      pNetworkManager->subscribeToOnBeforeMapChange(std::bind(&Atlas::onBeforeMapChange, pInstance, std::placeholders::_1));
      pNetworkManager->subscribeToOnMapChange(std::bind(&Atlas::onMapChange, pInstance, std::placeholders::_1));
      pNetworkManager->subscribeToRefreshClient(std::bind(&Atlas::onRefreshClientView, pInstance));
      pNetworkManager->subscribeToBlockQueryRequest(std::bind(&Atlas::onHashQuery, pInstance, std::placeholders::_1, std::placeholders::_2));
      pNetworkManager->subscribeToStaticsUpdate(std::bind(&Atlas::onUpdateStatics, pInstance, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
      pNetworkManager->subscribeToMapDefinitionUpdate(std::bind(&Atlas::onUpdateMapDefinitions, pInstance, std::placeholders::_1));
      pNetworkManager->subscribeToLandUpdate(std::bind(&Atlas::onUpdateLand, pInstance, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
      pNetworkManager->subscribeToUltimaLiveLoginComplete(std::bind(&Atlas::onShardIdentifierUpdate, pInstance, std::placeholders::_1));
      pNetworkManager->subscribeToLogout(std::bind(&Atlas::onLogout, pInstance));
    }

    pInstance->m_pFileManager = pFileManager;
    pInstance->m_pNetworkManager = pNetworkManager;
    pInstance->m_pClient = pClient;
  }
  else
  {
      Logger::g_pLogger->LogPrintError("Atlas: Failed to acquire Atlas Instance\n");
    success = false;
  }

  return success;
}
#pragma endregion

Atlas::Atlas()
  : m_pNetworkManager(NULL),
  m_pFileManager(NULL),
  m_pClient(NULL),
  m_mapDefinitions(),
  m_currentMap(0),
  m_shardIdentifier(),
  m_firstMapLoad(true)
{
  //do nothing
}

void Atlas::onShardIdentifierUpdate(std::string shardIdentifier)
{
  m_shardIdentifier = shardIdentifier;
}

void Atlas::onLogout()
{
  m_pFileManager->onLogout();
}

void Atlas::LoadMap(uint8_t map)
{
  Logger::g_pLogger->LogPrint("ON BEFORE LOAD MAP: %i\n", map);

  if (m_firstMapLoad)
  {
    m_firstMapLoad = false;
    m_pFileManager->InitializeShardMaps(m_shardIdentifier, m_mapDefinitions);
  }

  if (m_mapDefinitions.find((uint32_t)map) != m_mapDefinitions.end())
  {
    MapTileDefinition definition;

    definition.mapWidthInTiles = m_mapDefinitions[map].mapWidthInTiles;
    definition.mapHeightInTiles = m_mapDefinitions[map].mapHeightInTiles;
    definition.mapWrapWidthInTiles = m_mapDefinitions[map].mapWrapWidthInTiles;
    definition.mapWrapHeightInTiles = m_mapDefinitions[map].mapWrapHeightInTiles;

    m_pClient->SetMapDimensions(definition);
    m_pFileManager->LoadMap(map);
    m_currentMap = map;
  }
#ifdef DEBUG
  else 
  {
    Logger::g_pLogger->LogPrint("MAP DEFINITION NOT FOUND: %i\n", m_mapDefinitions.size());

    for (std::map<uint32_t, MapDefinition>::iterator itr = m_mapDefinitions.begin(); itr != m_mapDefinitions.end(); itr++)
    {
      Logger::g_pLogger->LogPrint("Map Definition: %i/%i\n", itr->first, itr->second.mapNumber);
    }
  }
#endif
}

void Atlas::onUpdateStatics(uint8_t mapNumber, uint32_t blockNumber, uint8_t* pData, uint32_t length)
{
  Logger::g_pLogger->LogPrint("Block: %i Map Number: %i Length: %i\n", blockNumber, mapNumber, length);
  m_pFileManager->writeStaticsBlock(mapNumber, blockNumber, pData, length);
  m_pClient->refreshClientStatics(blockNumber);
}

void Atlas::onUpdateLand(uint8_t mapNumber, uint32_t blockNumber, uint8_t* pLandData)
{
  m_pFileManager->updateLandBlock(mapNumber, blockNumber, pLandData);
  m_pClient->refreshClientLand();
}

void Atlas::onHashQuery(uint32_t blockNumber, uint8_t mapNumber)
{
  Logger::g_pLogger->LogPrint("Atlas: Got Hash Query\n");
  uint16_t* crcs = GetGroupOfBlockCrcs(mapNumber, blockNumber);
  uint8_t* pResponse = new uint8_t[71];

  pResponse[0] = 0x3F;                                               //byte 000              -  cmd
  *reinterpret_cast<uint16_t*>(pResponse + 1) = 71;                  //byte 001 through 002  -  packet size
  *reinterpret_cast<uint32_t*>(pResponse + 3) = htonl(blockNumber);  //byte 003 through 006  -  central block number for the query (block that player is standing in)
  *reinterpret_cast<uint32_t*>(pResponse + 7) = htonl(8);            //byte 007 through 010  -  number of statics in the packet (8 for a query response)
  *reinterpret_cast<uint16_t*>(pResponse + 11) = htons(0);           //byte 011 through 012  -  UltimaLive sequence number
  pResponse[13] = 0xFF;                                              //byte 013              -  UltimaLive command (0xFF is a block Query Response)
  pResponse[14] = mapNumber;                                         //byte 014              -  UltimaLive mapnumber
  //byte 015 through 64   -  25 block CRCs
  for (int i = 0; i < 25; i++)
  {
    *reinterpret_cast<uint16_t*>(pResponse + 15 + (i * 2)) = htons(crcs[i]);
  }

  pResponse[65] = 0xFF;                                           //byte 065              -  padding
  pResponse[66] = 0xFF;                                           //byte 066              -  padding
  pResponse[67] = 0xFF;                                           //byte 067              -  padding
  pResponse[68] = 0xFF;                                           //byte 068              -  padding
  pResponse[69] = 0xFF;                                           //byte 069              -  padding
  pResponse[70] = 0xFF;                                           //byte 070              -  padding

  m_pNetworkManager->sendPacketToServer(pResponse);

  delete pResponse;
}

void Atlas::onUpdateMapDefinitions(std::vector<MapDefinition> definitions)
{
  m_mapDefinitions.clear();

  for (std::vector<MapDefinition>::iterator itr = definitions.begin(); itr != definitions.end(); itr++)
  {
    m_mapDefinitions[itr->mapNumber] = *itr;
    Logger::g_pLogger->LogPrint("Registering Map #%i, dim=%ix%i, wrap=%ix%i\n", itr->mapNumber, itr->mapWidthInTiles, itr->mapHeightInTiles, itr->mapWrapWidthInTiles, itr->mapWrapHeightInTiles);
  }
}

void Atlas::onRefreshClientView()
{
  Logger::g_pLogger->LogPrint("Atlas: Refreshing Client View\n");
  uint8_t aPacketData[11];
  aPacketData[0] = (byte)0x21; //char move reject command
  aPacketData[1] = (byte)0xFF;  //seq num
  *reinterpret_cast<uint16_t*>(&aPacketData[2]) = 0;
  *reinterpret_cast<uint16_t*>(&aPacketData[4]) = 0;
  aPacketData[6] = 0;
  aPacketData[7] = 0;
  aPacketData[8] = (byte)0x22;
  aPacketData[9] = (byte)0x00;
  aPacketData[10] = (byte)0x00;
  m_pNetworkManager->sendPacketToClient(aPacketData);
  uint8_t aPacketData2[3];
  aPacketData2[0] = 0x22;
  aPacketData2[1] = 0xFF; //sequence number
  aPacketData2[2] = 0x00;
  m_pNetworkManager->sendPacketToClient(aPacketData2);
}

void Atlas::onBeforeMapChange(uint8_t&)
{
  //send a packet to tell the client to change to map 1
  uint8_t packet[6];
  packet[0] = 0xBF;
  packet[1] = 0x06;
  packet[2] = 0x00;
  packet[3] = 0x00;
  packet[4] = 0x08;
  packet[5] = 0x01;
  m_pNetworkManager->sendPacketToClient(packet);
}

void Atlas::onMapChange(uint8_t& rMap)
{
  //modify map change packet to point to map 0
  LoadMap(rMap);
  rMap = 0x00;
}

uint8_t Atlas::getCurrentMap()
{
  return m_currentMap;
}

uint16_t Atlas::fletcher16(uint8_t* pBlockData, uint8_t* pStaticsData, uint32_t staticsLength)
{
  uint16_t sum1 = 0;
  uint16_t sum2 = 0;

  if (pBlockData != NULL)
  {
    for (int index = 0; index < 192; ++index)
    {
      sum1 = (uint16_t)((sum1 + pBlockData[index]) % 255);
      sum2 = (uint16_t)((sum2 + sum1) % 255);
    }
  }

  if (pStaticsData != NULL)
  {
    for (uint32_t index = 0; index < staticsLength; ++index)
    {
      sum1 = (uint16_t)((sum1 + pStaticsData[index]) % 255);
      sum2 = (uint16_t)((sum2 + sum1) % 255);
    }
  }

  return (uint16_t)((sum2 << 8) | sum1);
}

int32_t Atlas::BLOCK_POSITION_OFFSETS[5] = { -2, -1, 0, 1, 2 };

uint16_t* Atlas::GetGroupOfBlockCrcs(uint32_t mapNumber, uint32_t blockNumber)
{
  uint16_t* pCrcs = NULL;
  if (m_mapDefinitions.find(mapNumber) != m_mapDefinitions.end())
  {
    pCrcs = new uint16_t[25];
    MapDefinition def = m_mapDefinitions[mapNumber];
    int32_t blockX = ((int32_t)blockNumber /  (def.mapHeightInTiles >> 3));
    int32_t blockY = ((int32_t)blockNumber % (def.mapHeightInTiles >> 3));

    int32_t wrapWidthInBlocks = (def.mapWrapWidthInTiles >> 3);
    int32_t wrapHeightInBlocks = (def.mapWrapHeightInTiles >> 3);
    int32_t mapWidthInBlocks = (def.mapWidthInTiles >> 3);
    int32_t mapHeightInBlocks = (def.mapHeightInTiles >> 3);

    if (blockX < 0 || blockX >= mapWidthInBlocks|| blockY < 0 || blockY >= mapHeightInBlocks)
    {
      memset(pCrcs, 0x00, sizeof(uint16_t) * 25);
    }
    else
    {
      for (int x = -2; x <= 2; x++)
      {
        int xBlockItr = -1;
        if (blockX < wrapWidthInBlocks)
        {
          xBlockItr = (blockX + x) % wrapWidthInBlocks;
          if (xBlockItr < 0 && xBlockItr > -3)
          {
            xBlockItr += wrapWidthInBlocks;
          }
        }
        else
        {
          xBlockItr = (blockX + x) % mapWidthInBlocks;
          if (xBlockItr < 0 && xBlockItr > -3)
          {
            xBlockItr += mapWidthInBlocks;
          }
        }
        for (int y = -2; y <= 2; y++)
        {
          int yBlockItr = 0;
          if (blockY < wrapHeightInBlocks)
          {
            yBlockItr = (blockY + y) % wrapHeightInBlocks;
            if (yBlockItr < 0)
            {
              yBlockItr += wrapHeightInBlocks;
            }
          }
          else
          {
            yBlockItr = (blockY + y) % mapHeightInBlocks;
            if (yBlockItr < 0)
            {
              yBlockItr += mapHeightInBlocks;
            }
          }

          int32_t currentBlock = (xBlockItr * mapHeightInBlocks) + yBlockItr;

          if (currentBlock >= 0 && currentBlock <= (mapHeightInBlocks * mapWidthInBlocks))
          {
            pCrcs[((x + 2) * 5) + (y + 2)] = getBlockCrc (mapNumber, currentBlock);
          }
          else
          {
            pCrcs[((x + 2) * 5) + (y + 2)] = (uint16_t)0x0;
          }
        }
      }
    }
  }

  return pCrcs;
}

uint16_t Atlas::getBlockCrc(uint32_t mapNumber, uint32_t blockNumber)
{
  uint16_t crc = 0; 
  if (m_mapDefinitions.find(mapNumber) != m_mapDefinitions.end())
  {
    uint8_t* pBlockData = m_pFileManager->readLandBlock(static_cast<uint8_t>(mapNumber), blockNumber);
    uint32_t staticsLength = 0;

    uint8_t* pStaticsData = m_pFileManager->readStaticsBlock(mapNumber, blockNumber, staticsLength);

    if (pBlockData != NULL)
    {
      crc = fletcher16(pBlockData, pStaticsData, staticsLength);
      delete pBlockData;
    }

    if (pStaticsData != NULL)
    {
      delete pStaticsData;
    }
  }

  return crc;
}

