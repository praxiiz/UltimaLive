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


#include "Client.h"
using namespace std;

void* Client::BASE_ADDRESS = reinterpret_cast<void*>(0x400000);

#pragma region Self Registration
SelfRegisteringClass <Client> Client::m_registration;

/**
 * @brief Self Registering Class Configure Method.  This function is called first  
 * to create an instance of this class and make it available for other
 * class instances to find.
 */
void Client::Configure()
{
  Logger::g_pLogger->LogPrint("Client configure\n");
  Client* pClient = new Client();
  UltimaLive::g_pUltimaLive->Register("Client", pClient);
  UltimaLive::g_pUltimaLive->m_pClient = pClient;
}

/**
 * @brief Self Registering Class Initialize Method. Searches for client internal structures and pointers. 
 *
 * @return True if all client structures are found
 */
bool Client::Initialize()
{
  bool success = true;
  Logger::g_pLogger->LogPrint("Atlas initialize\n");

  Client* pInstance = static_cast<Client*>(UltimaLive::g_pUltimaLive->Lookup("Client"));

  if (pInstance != NULL)
  {
    NetworkManager* pNetworkManager = static_cast<NetworkManager*>(UltimaLive::g_pUltimaLive->Lookup("NetworkManager"));
    if (pNetworkManager != NULL)
    {
      pInstance->m_pNetworkManager = pNetworkManager;

      BaseFileManager* pFileManager = static_cast<BaseFileManager*>(UltimaLive::g_pUltimaLive->Lookup("FileManager"));
      if (pFileManager != NULL)
      {
        pInstance->m_pFileManager = pFileManager;
        if (!pInstance->LookupClientInternalStructuresAndFunctions())
        {
          success = false;
          Logger::g_pLogger->LogPrintError("FAILED to find all internal client structures and pointers.\n");
        }
        else
        {
          Logger::g_pLogger->LogPrint("All internal client structures and pointers found.\n");
        }
      }
      else
      {
        Logger::g_pLogger->LogPrintError("Client: Failed to acquire FileManager Instance\n");
        success = false;
      }

    }
    else
    {
      Logger::g_pLogger->LogPrintError("Client: Failed to acquire NetworkManager Instance\n");
      success = false;
    }
  }
  else
  {
    Logger::g_pLogger->LogPrintError("Client: Failed to acquire Client Instance\n");
    success = false;
  }

  return success;
}
#pragma endregion

#pragma region Search Signatures for Client Internals
unsigned char Client::g_sendSignature1[] = { 0x8D, 0x8B, 0x94, 0x00, 0x00, 0x00, 0xE8, 0xCC, 0xCC, 0xCC, 0xCC, 0x55, 0x8D, 0x8B, 0xBC, 0x00, 0x00, 0x00 };
unsigned char Client::g_sendSignature2[] = { 0x0F, 0xB7, 0xD8, 0x0F, 0xB6, 0x06, 0x83, 0xC4, 0x04, 0x53, 0x50, 0x8D, 0x4F, 0x6C };
unsigned char Client::g_recvSignature1[] = { 0x53, 0x56, 0x57, 0x8B, 0xF9, 0x8B, 0x0D, 0xCC, 0xCC, 0xCC, 0xCC, 0x33, 0xD2 };
unsigned char Client::g_recvSignature2[] = { 0xA1, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xC7, 0x05, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x8B, 0x38, 0x8B, 0xCC, 0xBE };

unsigned char Client::g_NetworkObjectSignature1[] = { 0xA1, 0xCC, 0xCC, 0xCC, 0xCC, 0x8B, 0x0D, 0xCC, 0xCC, 0xCC, 0xCC, 0x8B, 0x16 };
unsigned char Client::g_NetworkObjectSignature2[] = { 0xC7, 0x06, 0xCC, 0xCC, 0xCC, 0xCC, 0x89, 0x35, 0xCC, 0xCC, 0xCC, 0xCC, 0x8B, 0x4C, 0x24, 0x0C };

unsigned char Client::g_PlayerBaseSignature1[] = { 0x66, 0x89, 0x71, 0x24, 0x8B, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC };
unsigned char Client::g_PlayerBaseSignature2[] = { 0x66, 0x89, 0x71, 0x28, 0x8B, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC };
unsigned char Client::g_mapDimensionSignature[] = { 0x00, 0x1c, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00 };
unsigned char Client::g_aDrawMapThingieTableSig1[] = { 0x83, 0xE1, 0x3F, 0x83, 0xE2, 0x3F, 0xC1, 0xE2, 0x06 }; 
unsigned char Client::g_aDrawMapThingieTableSig2[] = { 0x83, 0xE1, 0x3F, 0xC1, 0xE1, 0x06, 0x83, 0xE2, 0x3F }; 
unsigned char Client::g_minClientDisplaySig1[] = { 0xB8, 0xF1, 0xD8, 0xFF, 0xFF, 0x89, 0x3D, 0xCC, 0xCC, 0xCC, 0xCC, 0xA3, 0xCC, 0xCC, 0xCC, 0xCC, 0xA3 };
unsigned char Client::g_minClientDisplaySig2[] = { 0xB8, 0xF1, 0xD8, 0xFF, 0xFF, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xA3, 0xCC, 0xCC, 0xCC, 0xCC, 0xA3 };
unsigned char Client::g_minClientDisplaySig3[] = { 0xB8, 0xF1, 0xD8, 0xFF, 0xFF, 0xA3, 0xCC, 0xCC, 0xCC, 0xCC, 0xA3 };
unsigned char Client::g_updateBlocksSig1[] = { 0xEE, 0xC1, 0xCC, 0x03, 0xC1, 0xCC, 0x03 }; //works up through 7.0.1.1
unsigned char Client::g_updateBlocksSig2[] = { 0x2B, 0xD8, 0xC1, 0xCC, 0x03, 0xC1, 0xCC, 0x03 }; //works up through 7.0.1.1
unsigned char Client::g_refreshTerrainFunctionSig1[] = { 0x75, 0xCC, 0x83, 0xCC, 0xFF, 0x74, 0xCC, 0xE8 };
unsigned char Client::g_masterStaticsListSig1[] = { 0x33, 0xf6, 0x68, 0x00, 0x00, 0xCC, 0x00, 0x89 }; //2.0.8n to 7.0.8.2
unsigned char Client::g_masterStaticsListSig2[] = { 0x68, 0x40, 0xF9, 0x2F, 0x00, 0x89, 0x35 };
#pragma endregion

#pragma region Lookup Methods for Client Internals

/**
 * @brief Looks up the client's DrawMapThingie Structure
 *
 * @return Memory pointer to client's DrawMapThingie Structure
 */
unsigned char* Client::FindDrawMapThingieTable()
{
  unsigned char* drawMapThingieTableAddress = static_cast<unsigned char*>(MasterControlUtils::FindSignature(BASE_ADDRESS, 0x300000, g_aDrawMapThingieTableSig1, sizeof(g_aDrawMapThingieTableSig1), 14));
  if (drawMapThingieTableAddress == 0)
  {
    drawMapThingieTableAddress = static_cast<unsigned char*>(MasterControlUtils::FindSignature(BASE_ADDRESS, 0x300000, g_aDrawMapThingieTableSig2, sizeof(g_aDrawMapThingieTableSig2), 14));
  }

  return drawMapThingieTableAddress;
}

/**
 * @brief Searches for the client's Master Statics List Structure
 *
 * @return Memory pointer to the Master Statics List Structure
 */
CStaticObject** Client::FindMasterStaticsList()
{
  CStaticObject** masterStaticListAddress = static_cast<CStaticObject**>(MasterControlUtils::FindSignature(BASE_ADDRESS, 0x300000, g_masterStaticsListSig1, sizeof(g_masterStaticsListSig1), 9));
  if (masterStaticListAddress == 0)
  {
    masterStaticListAddress = static_cast<CStaticObject**>(MasterControlUtils::FindSignature(BASE_ADDRESS, 0x300000, g_masterStaticsListSig2, sizeof(g_masterStaticsListSig2), 7));
  }

  return masterStaticListAddress;
}

/**
 * @brief Searches for the client's Structure that holds the minimum coords to display on screen
 *
 * @return Memory pointer to the Minimum Coord Display Structure
 */
MapMinDisplay* Client::FindMapMinCoordDisplayStructure()
{
  unsigned char* minClientDisplayX = static_cast<unsigned char*>(MasterControlUtils::FindSignature(BASE_ADDRESS, 0x300000, g_minClientDisplaySig1, sizeof(g_minClientDisplaySig1), 12));

  if (minClientDisplayX == 0)
  {
    minClientDisplayX = static_cast<unsigned char*>(MasterControlUtils::FindSignature(BASE_ADDRESS, 0x300000, g_minClientDisplaySig2, sizeof(g_minClientDisplaySig2), 11));

    if (minClientDisplayX == 0)
    {
      minClientDisplayX = static_cast<unsigned char*>(MasterControlUtils::FindSignature(BASE_ADDRESS, 0x300000, g_minClientDisplaySig3, sizeof(g_minClientDisplaySig3), 6));
    }
  }

  return reinterpret_cast<MapMinDisplay*>(minClientDisplayX);
}

/**
 * @brief Searches for the client's Display Blocks Table.  This table contains a 64x64 array of blocks.
 *
 * @return Pointer to an the clients Display Blocks Table
 */
int* Client::FindClientDisplayedBlocksTable()
{
  void* minClientDisplayX = MasterControlUtils::FindSignature(BASE_ADDRESS, 0x300000, g_minClientDisplaySig1, sizeof(g_minClientDisplaySig1), 12);

  if (minClientDisplayX == 0)
  {
    minClientDisplayX = MasterControlUtils::FindSignature(BASE_ADDRESS, 0x300000, g_minClientDisplaySig2, sizeof(g_minClientDisplaySig2), 11);

    if (minClientDisplayX == 0)
    {
      minClientDisplayX = MasterControlUtils::FindSignature(BASE_ADDRESS, 0x300000, g_minClientDisplaySig3, sizeof(g_minClientDisplaySig3), 6);
    }
  }

  if (minClientDisplayX != 0)
  {
    minClientDisplayX = static_cast<char*>(minClientDisplayX) - (36 * 4);
  }

  return static_cast<int*>(minClientDisplayX);
}

/**
 * @brief Searches for the client function that updates onscreen statics
 *
 * @return Pointer to the UpdateBlocks client function
 */
Client::ClientUpdateStaticBlocksFunction Client::FindUpdateBlocksFunction()
{
  void* updateBlock = MasterControlUtils::FindFunctionCall(BASE_ADDRESS, 0x300000, g_updateBlocksSig1, sizeof(g_updateBlocksSig1));

  if (updateBlock == 0)
  {
    updateBlock = static_cast<unsigned char*>(MasterControlUtils::FindFunctionCall(BASE_ADDRESS, 0x300000, g_updateBlocksSig2, sizeof(g_updateBlocksSig2)));
  }

  return static_cast<ClientUpdateStaticBlocksFunction>(updateBlock);
}

/**
 * @brief Searches for the client function that refreshes the onscreen map graphics.
 *
 * @return Pointer to the refresh terrain client function
 */
Client::RefreshTerrainFunction Client::FindRefreshTerrainFunction()
{
  void* pRefreshFunction = 0;

  char* func1SigAddr = reinterpret_cast<char*>(MasterControlUtils::FindSignatureOffset(BASE_ADDRESS, CLIENT_IMAGE_MAX_SIZE, g_refreshTerrainFunctionSig1, sizeof(g_refreshTerrainFunctionSig1)));

  if (func1SigAddr != 0)
  {
    int func1Offset = *reinterpret_cast<int*>(func1SigAddr + 8);
    pRefreshFunction = func1SigAddr + func1Offset + 12;
  }

  return reinterpret_cast<Client::RefreshTerrainFunction>(pRefreshFunction);
}

/**
 * @brief Searches for the client Player structure memory address
 *
 * @return Pointer to the memory address of the client player structure
 */
CPlayerMobile* Client::FindPlayerStructure()
{
  void* pOffset = MasterControlUtils::FindSignatureOffset(BASE_ADDRESS, CLIENT_IMAGE_MAX_SIZE, g_PlayerBaseSignature1, sizeof(g_PlayerBaseSignature1));

  if(pOffset==0)
  {
    pOffset = MasterControlUtils::FindSignatureOffset(BASE_ADDRESS, CLIENT_IMAGE_MAX_SIZE, g_PlayerBaseSignature2, sizeof(g_PlayerBaseSignature2));
  }

  return reinterpret_cast<CPlayerMobile*>(static_cast<char*>(pOffset) + 6);
}

/**
 * @brief Searches for the client's network send function
 *
 * @return Pointer to the memory address of the client's network send function
 */
Client::ClientSendFunction Client::FindSendFunction()
{
  unsigned char* pReturnAddress = NULL;

  void* pOffset =  MasterControlUtils::FindSignatureOffset(BASE_ADDRESS, CLIENT_IMAGE_MAX_SIZE, g_sendSignature1, sizeof(g_sendSignature1));

  if (pOffset != 0)
  {
    pReturnAddress = static_cast<unsigned char*>(pOffset) - 0x22;
  }
  else
  {
    pOffset = MasterControlUtils::FindSignatureOffset(BASE_ADDRESS, CLIENT_IMAGE_MAX_SIZE, g_sendSignature2, sizeof(g_sendSignature2));
    if (pOffset != 0)
    {
      pReturnAddress = static_cast<unsigned char*>(pOffset) - 0xF;
    }
  }

  return reinterpret_cast<ClientSendFunction>(pReturnAddress);
}

/**
 * @brief Searches for the client's network receive function
 *
 * @return Pointer to the memory address of the client's network receive function
 */
Client::ClientRecvFunction Client::FindRecvFunction()
{
  unsigned char* pReturnAddress = NULL;
  void* pOffset =  MasterControlUtils::FindSignatureOffset(BASE_ADDRESS, CLIENT_IMAGE_MAX_SIZE, g_recvSignature1, sizeof(g_recvSignature1));

  if (pOffset != 0)
  {
    pReturnAddress = (unsigned char*)pOffset;
  }
  else
  {
    pOffset =  MasterControlUtils::FindSignatureOffset(BASE_ADDRESS, CLIENT_IMAGE_MAX_SIZE, g_recvSignature2, sizeof(g_recvSignature2));

    if (pOffset != 0)
    {
      pReturnAddress = (unsigned char*)pOffset;
    }
  }

  return reinterpret_cast<ClientRecvFunction>(pReturnAddress);
}

/**
 * @brief Searches for the client's network manager
 *
 * @return Pointer to the memory address of the client's network manager
 */
void** Client::FindNetworkStructureInstance()
{
  void** pNetworkObject = NULL;
  void* pOffset =  static_cast<void**>(MasterControlUtils::FindSignatureOffset(BASE_ADDRESS, CLIENT_IMAGE_MAX_SIZE, g_NetworkObjectSignature1, sizeof(g_NetworkObjectSignature1)));
  if (pOffset == 0)
  {
    pOffset =  MasterControlUtils::FindSignatureOffset(BASE_ADDRESS, CLIENT_IMAGE_MAX_SIZE, g_NetworkObjectSignature2, sizeof(g_NetworkObjectSignature2));
    if (pOffset != 0)
    {
      pNetworkObject = reinterpret_cast<void**>(static_cast<unsigned char*>(pOffset) + 8);
      pNetworkObject = reinterpret_cast<void**>(*pNetworkObject);
    }
  }
  else
  {
    pNetworkObject = reinterpret_cast<void**>(static_cast<unsigned char*>(pOffset) + 7);
    pNetworkObject = reinterpret_cast<void**>(*pNetworkObject);
  }

  return pNetworkObject;
}

/**
 * @brief Searches for the client's Map Dimension Structure. The Map Dimension Structure contains the map dimensions and wrapping coordinates for each client map. 
 *
 * @return Pointer to the memory address of the client's Map Dimension Structure
 */
MapTileDefinition* Client::FindMapDimensionStructure()
{
  return reinterpret_cast<MapTileDefinition*>(MasterControlUtils::FindSignatureOffset(BASE_ADDRESS, CLIENT_IMAGE_MAX_SIZE, g_mapDimensionSignature, sizeof(g_mapDimensionSignature)));
}
#pragma endregion

/**
 * @brief Checks to see if the resulting pointer exists in the memory space of the client.
 *
 * @return True if the memory address is valid, false otherwise
 */
bool Client::checkLookupResult(void* pAddress)
{
  bool success = true;
  if (pAddress < BASE_ADDRESS)
  {
    Logger::g_pLogger->LogPrintWithoutDate(" 0x%08x ", 0);
    Logger::g_pLogger->LogPrintTaskStatusResult(false);
    success = false;
  }
  else
  {
    Logger::g_pLogger->LogPrintWithoutDate(" 0x%08x ", pAddress);
    Logger::g_pLogger->LogPrintTaskStatusResult(true);
  }

  return success;
}

/**
 * @brief Client class constructor
 */
Client::Client()
  : m_pNetworkStructureInstance(NULL),
  m_pRecvFunction(NULL),
  m_pSendFunction(NULL),
  m_pMapThingieTable(NULL),
  m_pMapDimensionsStructure(NULL),
  m_pMapMinCoordDisplayStructure(NULL),
  m_pClientDisplayedBlocksTable(NULL),
  m_pPlayerStructure(NULL),
  m_pUpdateBlocksFunction(NULL),
  m_pRefreshTerrainFunction(NULL),
  m_pMasterStaticsList(NULL),
  m_pDotNetHost(NULL),
  m_pNetworkManager(NULL),
  m_pFileManager(NULL)
{
  //do nothing
}

/**
 * @brief Search out all internal structure addresses.
 * 
 * @return true if all pointers and internal structures are found
 */
bool Client::LookupClientInternalStructuresAndFunctions()
{
  Logger::g_pLogger->LogPrint("Checking to see if this client is compatible...\n");
  Logger::g_pLogger->LogPrint("Searching for internal client structures and pointers\n");

  bool success = true;

  Logger::g_pLogger->LogPrintWithoutDate("%-75s", "   Network Class Instance");
  m_pNetworkStructureInstance = FindNetworkStructureInstance();
  success = checkLookupResult(m_pNetworkStructureInstance);

  Logger::g_pLogger->LogPrintWithoutDate("%-75s", "   Receive Function");
  m_pRecvFunction = FindRecvFunction();
  success = checkLookupResult(m_pRecvFunction);

  Logger::g_pLogger->LogPrintWithoutDate("%-75s", "   Send Function");
  m_pSendFunction = FindSendFunction();
  success = checkLookupResult(m_pSendFunction);

  Logger::g_pLogger->LogPrintWithoutDate("%-75s", "   DrawMapThingie Table");
  m_pMapThingieTable = FindDrawMapThingieTable();
  success = checkLookupResult(m_pMapThingieTable);

  Logger::g_pLogger->LogPrintWithoutDate("%-75s", "   Map Dimension Structure");
  m_pMapDimensionsStructure = FindMapDimensionStructure();
  success = checkLookupResult(m_pMapDimensionsStructure);

  Logger::g_pLogger->LogPrintWithoutDate("%-75s", "   Min Coord Client Display Structure");
  m_pMapMinCoordDisplayStructure = FindMapMinCoordDisplayStructure();
  success = checkLookupResult(m_pMapMinCoordDisplayStructure);

  Logger::g_pLogger->LogPrintWithoutDate("%-75s", "   Displayed Blocks Table");
  m_pClientDisplayedBlocksTable = FindClientDisplayedBlocksTable();
  success = checkLookupResult(m_pClientDisplayedBlocksTable);

  Logger::g_pLogger->LogPrintWithoutDate("%-75s", "   Player Structure");
  m_pPlayerStructure = FindPlayerStructure();
  success = checkLookupResult(m_pPlayerStructure);

  Logger::g_pLogger->LogPrintWithoutDate("%-75s", "   Update Blocks Function");
  m_pUpdateBlocksFunction = FindUpdateBlocksFunction();
  success = checkLookupResult(m_pUpdateBlocksFunction);

  Logger::g_pLogger->LogPrintWithoutDate("%-75s", "   Refresh Terrain Function");
  m_pRefreshTerrainFunction = FindRefreshTerrainFunction();
  success = checkLookupResult(m_pRefreshTerrainFunction);

  Logger::g_pLogger->LogPrintWithoutDate("%-75s", "   Master Statics List");
  m_pMasterStaticsList = FindMasterStaticsList();
  success = checkLookupResult(m_pMasterStaticsList);

  return success;
}

/**
 * @brief Getter for client's Receive Function pointer
 */
Client::ClientRecvFunction& Client::getRecvFunction()
{
  return m_pRecvFunction;
}

/**
 * @brief Getter for client's Send Function pointer
 */
Client::ClientSendFunction& Client::getSendFunction()
{
  return m_pSendFunction;
}

/**
 * @brief Getter for client's Update Statics Blocks Function pointer
 */
Client::ClientUpdateStaticBlocksFunction& Client::getUpdatesStaticBlocksFunction()
{
  return m_pUpdateBlocksFunction;
}

/**
 * @brief Refreshes statics on the client screen for a given block
 *
 * @param blockNumber block number to refresh
 */
void Client::refreshClientStatics(uint32_t blockNumber)
{
  Logger::g_pLogger->LogPrint("Refreshing Client View: %i\n", blockNumber);

  //PURGE MASTER STATIC LIST
  for (CStaticObject* pStaticItem = *(CStaticObject**)m_pMasterStaticsList; pStaticItem != NULL && pStaticItem->drawItemMembers.bitPattern == 0xFEEDBEEF; pStaticItem = pStaticItem->staticObjectMembers.pNextStaticObject)
  {
    uint32_t staticBlockNumber = ((pStaticItem->drawItemMembers.x >> 3) * (m_pMapDimensionsStructure->mapHeightInTiles >> 3)) + (pStaticItem->drawItemMembers.y >> 3);
    if (staticBlockNumber == blockNumber && !pStaticItem->pVtable->drawItemVtableMembers.IsDynamic(pStaticItem))
    {
      if (pStaticItem->drawItemMembers.inDrawList != 0)
      {
        pStaticItem->pVtable->drawItemVtableMembers.RemoveFromDrawList(pStaticItem);
      }       
    }
  }

  //Clear Client Blocks Array
  for (int i = 0; i < 36; i++)
  {
    if ( reinterpret_cast<uint32_t*>(m_pClientDisplayedBlocksTable)[i] == blockNumber)
    {
      reinterpret_cast<int*>(m_pClientDisplayedBlocksTable)[i] = -1;
    }
  }

  //Set Client Minimum Displayed X to -9999
  m_pMapMinCoordDisplayStructure->minClientDisplayX = static_cast<uint32_t>(-9999);
  m_pMapMinCoordDisplayStructure->minClientDisplayY = static_cast<uint32_t>(-9999);

  ClientRedirections::OnUpdateStaticBlocks();
}

/**
 * @brief Refreshes the land tiles on the client screen
 */
void Client::refreshClientLand()
{
  for (int x = 0; x < 64; ++x)
  {
    for (int y = 0; y < 64; ++y)
    {
      CDrawItem** pDrawItem = reinterpret_cast<CDrawItem**>(m_pMapThingieTable + ( (((y & 0x3F) * 64) + (x & 0x3F)) * sizeof(uint32_t)));

      if (pDrawItem != NULL && *pDrawItem != NULL)
      {
        for (CDrawItem* pDrawItr = *pDrawItem; pDrawItr != NULL && pDrawItr->drawItemMembers.bitPattern == 0xFEEDBEEF; pDrawItr = pDrawItr->drawItemMembers.pNextDraw) 
        {
          if (pDrawItr->pVtable->drawItemVtableMembers.IsTerrain(pDrawItr) != 0)
          {
            pDrawItr->pVtable->drawItemVtableMembers.RemoveFromDrawList(pDrawItr);
          }
        }
      }
    }

    m_pRefreshTerrainFunction();
  }
}

/**
 * @brief Sends an array of bytes to the server using the clients send function
 *
 * @param pBuffer pointer to byte array
 */
void Client::SendPacketToServer(unsigned char* pBuffer)
{
  m_pSendFunction(*m_pNetworkStructureInstance, pBuffer);
}

/**
 * @brief Sends an array of bytes to the client using the clients receive function
 *
 * @param pBuffer pointer to byte array
 */
void Client::SendPacketToClient(unsigned char* pBuffer)
{
  Logger::g_pLogger->LogPrint("Sending Packet to Client!\n");
  m_pRecvFunction(*m_pNetworkStructureInstance, pBuffer);
}

/** 
 * @brief Updates the client's map dimensions
 *
 * @param definition New map definition set
 */
void Client::SetMapDimensions(MapTileDefinition definition)
{
  *m_pMapDimensionsStructure = definition;
}

/**
 * @brief Getter for the pointer to the client Player structure
 *
 * @return Client Player Structure Pointer
 */
CPlayerMobile* Client::getPlayerMobile()
{
  return m_pPlayerStructure;
}

/**
 * @brief Getter for the client Network Manager Pointer
 *
 * @return pointer to the client's Network Manager
 */
NetworkManager* Client::GetNetworkManager()
{
  return m_pNetworkManager;
}

/** 
 * @brief Getter for the pointer to the client's File Manager
 *
 * @return pointer to the client's File Manager
 */
BaseFileManager* Client::GetFileManager()
{
  return m_pFileManager;
}
