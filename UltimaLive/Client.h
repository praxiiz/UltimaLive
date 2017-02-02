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

#ifndef _CLIENT_H
#define _CLIENT_H

#include <windows.h>
#include <string>
#include <iostream>
#include <stdint.h>

#include "UltimaLive.h"
#include "LocalPeHelper32.hpp"
#include "mhook.h"
#include "MasterControlUtils.h"
#include "DotNetHost.h"
#include "Debug.h"
#include "ClassRegistration\SelfRegisteringClass.h"
#include "ClientStructures.h"

#include "FileSystem\BaseFileManager.h"
#include "Network\NetworkManager.h"

/* This class knows about the client internals. 
 * This class assumes that it shares memory space with the actual client, therefor no calls to get memory
 * from an external process are used.
 */
class Client
{
  private:
    static void* BASE_ADDRESS;
    static const int CLIENT_IMAGE_MAX_SIZE = 0x300000;

  #pragma region Client Internal APIs 
  public:
    typedef void (__thiscall* ClientRecvFunction)(void* This, unsigned char* pBuffer);
    typedef int (__thiscall* ClientSendFunction)(void* This, unsigned char* pBuffer);
    typedef void (__cdecl* ClientUpdateStaticBlocksFunction)();
    typedef int(__cdecl* RefreshTerrainFunction)();
  #pragma endregion

  #pragma region Search Signatures for Client Internals
  private:
    static unsigned char g_sendSignature1[];
	static unsigned char g_sendSignature2[];
	static unsigned char g_recvSignature1[];
	static unsigned char g_recvSignature2[];
    static unsigned char g_NetworkObjectSignature1[];
    static unsigned char g_NetworkObjectSignature2[];
    static unsigned char g_PlayerBaseSignature1[];
    static unsigned char g_PlayerBaseSignature2[];
    static unsigned char g_mapDimensionSignature[];
    static unsigned char g_aDrawMapThingieTableSig1[];
    static unsigned char g_aDrawMapThingieTableSig2[]; 
    static unsigned char g_minClientDisplaySig1[];
    static unsigned char g_minClientDisplaySig2[]; 
    static unsigned char g_minClientDisplaySig3[]; 
    static unsigned char g_updateBlocksSig1[];
    static unsigned char g_updateBlocksSig2[];
    static unsigned char g_refreshTerrainFunctionSig1[];
    static unsigned char g_masterStaticsListSig1[];
    static unsigned char g_masterStaticsListSig2[];
  #pragma endregion

  #pragma region Lookup Methods for Client Internals 
  private:
	static void** FindNetworkStructureInstance();
    static ClientRecvFunction FindRecvFunction();
    static ClientSendFunction FindSendFunction();
    static unsigned char* FindDrawMapThingieTable();
    static MapTileDefinition* FindMapDimensionStructure();
    static MapMinDisplay* FindMapMinCoordDisplayStructure();
    static int* FindClientDisplayedBlocksTable();
    static CPlayerMobile* FindPlayerStructure();
    static ClientUpdateStaticBlocksFunction FindUpdateBlocksFunction();
    static RefreshTerrainFunction FindRefreshTerrainFunction();
    static CStaticObject** FindMasterStaticsList();
  #pragma endregion

  #pragma region Pointers to Client Internals
  public:
    ClientRecvFunction m_pRecvFunction;
    ClientSendFunction m_pSendFunction;
    ClientUpdateStaticBlocksFunction m_pUpdateBlocksFunction;

  private:
    void** m_pNetworkStructureInstance;
    unsigned char* m_pMapThingieTable; //(CDrawItem*)* mapDrawThingie[64][64]
    MapTileDefinition* m_pMapDimensionsStructure;
    MapMinDisplay* m_pMapMinCoordDisplayStructure;
    int* m_pClientDisplayedBlocksTable;
    CPlayerMobile* m_pPlayerStructure;
    RefreshTerrainFunction m_pRefreshTerrainFunction;
    CStaticObject** m_pMasterStaticsList;
  #pragma endregion

  #pragma region Self Registration
  public:
    static bool Initialize();
    static void Configure();

  private:
    static SelfRegisteringClass <Client> m_registration;
  #pragma endregion

  public:
    Client();
    void SendPacketToServer(unsigned char* pBuffer);
    void SendPacketToClient(unsigned char* pBuffer);
    void refreshClientLand();
    void refreshClientStatics(uint32_t blockNumber);
    void SetMapDimensions(MapTileDefinition definition);

    CPlayerMobile* getPlayerMobile();
    bool LookupClientInternalStructuresAndFunctions();

    ClientRecvFunction& getRecvFunction();
    ClientSendFunction& getSendFunction();
    ClientUpdateStaticBlocksFunction& getUpdatesStaticBlocksFunction();

    NetworkManager* GetNetworkManager();
    BaseFileManager* GetFileManager();

  private:
    DotNetHost* m_pDotNetHost;

    bool checkLookupResult(void* address);

    NetworkManager* m_pNetworkManager;
    BaseFileManager* m_pFileManager;
};
#endif