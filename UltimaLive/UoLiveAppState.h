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

#ifndef _UO_LIVE_APP_STATE_H
#define _UO_LIVE_APP_STATE_H

#include <stdint.h>
#include <string>
#include <map>
#include <Windows.h>
#include "Utils.h"
#include "ClientRedirections.h"
#include "Network\NetworkManager.h"
#include "FileSystem\BaseFileManager.h"
#include "FileSystem\FileManagerFactory.h"
#include "Maps\Atlas.h"
#include "LoginHandler.h"
#include "MasterControlUtils.h"

class PlayerLocation
{
  public:
    uint16_t X;
    uint16_t Y;
    uint8_t Z;
    uint8_t Facing;
};

class UoLiveAppState
{
  public:
    UoLiveAppState();
    void Init();
    BaseFileManager* GetFileManager();
    NetworkManager* GetNetworkManager();
    Atlas* GetAtlas();

    void setPlayerLocation(PlayerLocation location);
    PlayerLocation getPlayerLocation();

    void setClientPlayerStructure(uint8_t* pPlayer);
    uint8_t* m_pMapDimensions;

  private:
    BaseFileManager* m_pFileManager;
    NetworkManager* m_pNetworkManager;
    uint32_t m_versionMajor;
    uint32_t m_versionMinor;
    Atlas* m_pAtlas;
    PlayerLocation m_playerLocation;
    std::map<uint8_t, uint8_t> m_movementRequests;
    uint8_t* m_pClientPlayerStructure;
    LoginHandler* m_pLoginHandler;
};

#endif