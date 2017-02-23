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

#ifndef _ATLAS_H
#define _ATLAS_H

#include <string>
#include <vector>
#include <windows.h>
#include <stdint.h>
#include <map>

#include "..\FileSystem\BaseFileManager.h"
#include "..\LocalPeHelper32.hpp"
#include "..\ClassRegistration\SelfRegisteringClass.h"
#include "..\Client.h"

#include "MapDefinition.h"

class UltimaLive;
class NetworkManager;
class LoginHandler;

/**
 * @class Atlas
 *
 * @brief Manages changing client maps
 */
class Atlas
{
  public:
    Atlas();
    uint16_t* GetGroupOfBlockCrcs(uint32_t mapNumber, uint32_t blockNumber);

    static uint16_t fletcher16(uint8_t* pBlockData, uint8_t* pStaticsData, uint32_t staticsLength);

    void LoadMap(uint8_t map);
    uint8_t getCurrentMap();


  protected:
    void onBeforeMapChange(uint8_t& rMap);
    void onMapChange(uint8_t& rMap);

    void onHashQuery(uint32_t blockNumber, uint8_t mapNumber);
    void onRefreshClientView();
    void onUpdateMapDefinitions(std::vector<MapDefinition> definitions);
    void onUpdateStatics(uint8_t mapNumber, uint32_t blockNumber, uint8_t* pData, uint32_t length);
    void onShardIdentifierUpdate(std::string shardIdentifier);

    void onUpdateLand(uint8_t mapNumber, uint32_t blockNumber, uint8_t* pLandData);

    void onLogout();

    static int32_t BLOCK_POSITION_OFFSETS[5];  //!< Array of offsets used when calculating hashes

    uint16_t getBlockCrc(uint32_t mapNumber, uint32_t blockNumber);


    std::map<uint32_t, MapDefinition> m_mapDefinitions; //!< Map linking map indices to map definitions 
    uint8_t m_currentMap; //!< Current map in client memory

    NetworkManager* m_pNetworkManager; //!< Pointer to Network Manager
    BaseFileManager* m_pFileManager;   //!< Pointer to File Manager 
    Client* m_pClient; //!< Pointer to Client Manager

    std::string m_shardIdentifier; //!< Unique Shard Identifier
    bool m_firstMapLoad; //!< Indicates if the map has been loaded in the past

#pragma region Self Registration
  public:
    static bool Initialize();
    static void Configure();

  private:
    static SelfRegisteringClass <Atlas> m_registration; //!< Class Self Registration mechanism
#pragma endregion
};

#endif