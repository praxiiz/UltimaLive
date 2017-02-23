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

/**
 * @class Client
 *
 * @brief This class knows about the client internals. 
 * This class assumes that it shares memory space with the actual client, therefor no calls to get memory
 * from an external process are used.
 */
class Client
{
  private:
    static void* BASE_ADDRESS; //!< Base address of client module in process virtual address space
    static const int CLIENT_IMAGE_MAX_SIZE = 0x300000; //!< Maximum search size inside the client

  #pragma region Client Internal APIs 
  public:
	/**
	 * @typedef ClientRecvFunction
	 *
	 * @brief Client Receive Function type
	 */
    typedef void (__thiscall* ClientRecvFunction)(void* This, unsigned char* pBuffer);

	/**
	 * @typedef ClientSendFunction
	 *
	 * @brief Client Send Function type
	 */
	typedef int (__thiscall* ClientSendFunction)(void* This, unsigned char* pBuffer);

	/**
	 * @typedef ClientUpdateStaticBlocksFunction
	 *
	 * @brief Client Update Statics Block function type
	 */
	typedef void (__cdecl* ClientUpdateStaticBlocksFunction)();

	/**
	 * @typedef RefreshTerrainFunction
	 * 
	 * @brief Refresh Terrain Function type
	 */
	typedef int(__cdecl* RefreshTerrainFunction)();
  #pragma endregion

  #pragma region Search Signatures for Client Internals
  private:
    static unsigned char g_sendSignature1[];			 //!< Bytes used to search for client's send function 
	static unsigned char g_sendSignature2[];			 //!< Bytes used to search for client's send function 
	static unsigned char g_recvSignature1[];			 //!< Bytes used to search for client's receive function 
	static unsigned char g_recvSignature2[];			 //!< Bytes used to search for client's receive function 
    static unsigned char g_NetworkObjectSignature1[];	 //!< Bytes used to search for client's Network Object 
    static unsigned char g_NetworkObjectSignature2[];	 //!< Bytes used to search for client's Network Object 
    static unsigned char g_PlayerBaseSignature1[];		 //!< Bytes used to search for client's Player instance
    static unsigned char g_PlayerBaseSignature2[];		 //!< Bytes used to search for client's Player instance
    static unsigned char g_mapDimensionSignature[];		 //!< Bytes used to search for client's Map Dimension Structure
    static unsigned char g_aDrawMapThingieTableSig1[];	 //!< Bytes used to search for client's Map Thingie Structure
    static unsigned char g_aDrawMapThingieTableSig2[]; 	 //!< Bytes used to search for client's Map Thingie Structure
    static unsigned char g_minClientDisplaySig1[];		 //!< Bytes used to search for client's Client Display Structure
    static unsigned char g_minClientDisplaySig2[]; 		 //!< Bytes used to search for client's Client Display Structure
    static unsigned char g_minClientDisplaySig3[]; 		 //!< Bytes used to search for client's Client Display Structure
    static unsigned char g_updateBlocksSig1[];			 //!< Bytes used to search for client's Update Block function 
    static unsigned char g_updateBlocksSig2[];			 //!< Bytes used to search for client's Update Block function 
    static unsigned char g_refreshTerrainFunctionSig1[]; //!< Bytes used to search for client's Refresh Terrain function 
    static unsigned char g_masterStaticsListSig1[];      //!< Bytes used to search for client's Master Statics List Structure
    static unsigned char g_masterStaticsListSig2[];      //!< Bytes used to search for client's Master Statics List Structure
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
    ClientRecvFunction m_pRecvFunction; //!< Pointer to client's receive function
    ClientSendFunction m_pSendFunction; //!< Pointer to client's send function
    ClientUpdateStaticBlocksFunction m_pUpdateBlocksFunction; //!< Pointer to client's update blocks function

  private:
    void** m_pNetworkStructureInstance;               //!< Pointer to client's Network Structure instance
    unsigned char* m_pMapThingieTable;                //!< Pointer to client's mapDrawThingie[64][64] (CDrawItem*)*
    MapTileDefinition* m_pMapDimensionsStructure;     //!< Pointer to client's Map Dimension Structure instance
    MapMinDisplay* m_pMapMinCoordDisplayStructure;    //!< Pointer to client's Mininum Coordinate Display Structure
    int* m_pClientDisplayedBlocksTable;               //!< Pointer to client's Displayed Blocks Table
    CPlayerMobile* m_pPlayerStructure;				  //!< Pointer to client's Player Class Instance
    RefreshTerrainFunction m_pRefreshTerrainFunction; //!< Pointer to client's Refresh Terrain function
    CStaticObject** m_pMasterStaticsList;			  //!< Pointer to client's Master Statics List
  #pragma endregion

  #pragma region Self Registration
  public:
    static bool Initialize();
    static void Configure();

  private:
    static SelfRegisteringClass <Client> m_registration; //!< Self Registration mechanism
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
    DotNetHost* m_pDotNetHost;  //!< Pointer to Dot Net host (currently unused)

    bool checkLookupResult(void* address);

    NetworkManager* m_pNetworkManager; //!< Local Pointer to global network manager instance
    BaseFileManager* m_pFileManager;   //!< Local Pointer to global file manager instance
};
#endif