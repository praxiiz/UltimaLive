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


/*

 These classes trigger events on the Network Manager
 +------------------------------------------------+                +------------------------------------------------------+
 |                BasePacketHandler               |                | Network Manager Events                               |
 +------------------------------------------------+                |------------------------------------------------------|
   ^    ^     ^    ^                                               | Network Events:                                      |
   |    |     |    |                                     +--------+|   UO Live Logic:                                     |
   |    |     |    |                                     | +------+|  x  Map Definitions Update                           |
   |    |     |    |                                     | |+-----+|  x  Land Update                                      |
   |    |     | +--+------------------------------+      | ||+----+|  x  Statics Update                                   |
   |    |     | | Login Confirm Handler 7_0_29_2  +------+ |||     |  x  Refresh Client View                              |
   |    |     | +---------------------------------+        |||     |  x  Block Query Request                              |
   |    |     |                                            |||     |  x  Login Complete (UO Live)                         |
   |    |   +-+-----------------------------------+        |||     |                                                      |
   |    |   |     Login Complete Handler 7_0_29_2 |--------+||     |                                                      |
   |    |   +-------------------------------------+         ||     |   Regular Game Logic:                                |
   |    |                                                   ||     |     Login Confirm                                    |
   |  +-+-----------------------------------------+         ||     |     Login Complete                                   |
   |  |           Change Map Handler 7_0_29_2     |---------+|     |     On Before Change Map                             |
   |  +-------------------------------------------+          |     |     Logout Request Sent                              |
   |                                                         |     +------------------------------------------------------+
 +-+----------------------------------------------+          |
 |                Logout Request Handler 7_0_29_2 |----------+
 +------------------------------------------------+
*/

#ifndef _NETWORK_MANAGER_H
#define _NETWORK_MANAGER_H

#include <string>
#include <vector>
#include <functional>
#include <Windows.h>
#include "PacketHandlerFactory.h"
#include "..\LocalPeHelper32.hpp"
#include "..\ClassRegistration\SelfRegisteringClass.h"
#include "..\Maps\MapDefinition.h"

class UltimaLive;
class Client;

/**
 * @class NetworkManager
 *
 * @brief Handles network packets going to/from the server/client
 */
class NetworkManager
{
  public:
    NetworkManager();

    bool OnReceivePacket(unsigned char *pBuffer);
    bool OnSendPacket(unsigned char *pBuffer);

    void sendPacketToClient(uint8_t* pBuffer);
    void sendPacketToServer(uint8_t* pBuffer);

    void onMapDefinitionUpdate(std::vector<MapDefinition> definitions);
    void onLandUpdate(uint8_t mapNumber, uint32_t blockNumber, uint8_t* pLandData);
    void onStaticsUpdate(uint8_t mapNumber, uint32_t blockNumber, uint8_t* pStaticsData, uint32_t length);
    void onRefreshClient();
    void onBlockQueryRequest(int32_t blockNumber, uint8_t mapNumber);
    void onUltimaLiveLoginComplete(std::string shardIdentifier);

    void onServerMobileUpdate();
    void onLoginConfirm(uint8_t* pData);
    void onLoginComplete();
    void onBeforeMapChange(uint8_t& mapNumber); //return true to let specific handler code run
    void onMapChange(uint8_t& mapNumber); //return true to allow regular map change packet to go through
    void onLogout();

    void subscribeToMapDefinitionUpdate(std::function<void(std::vector<MapDefinition>)> pCallback);
    void subscribeToLandUpdate(std::function<void(uint8_t, uint32_t, uint8_t*)> pCallback);
    void subscribeToStaticsUpdate(std::function<void(uint8_t, uint32_t, uint8_t*, uint32_t)> pCallback);
    void subscribeToRefreshClient(std::function<void()> pCallback);
    void subscribeToBlockQueryRequest(std::function<void(int32_t, uint8_t)> pCallback);
    void subscribeToUltimaLiveLoginComplete(std::function<void(std::string)> pCallback);

    void subscribeToServerMobileUpdate(std::function<void()> pCallback);
    void subscribeToLoginConfirm(std::function<void(uint8_t*)> pCallback);
    void subscribeToLoginComplete(std::function<void()> pCallback);
    void subscribeToOnBeforeMapChange(std::function<void(uint8_t&)> pCallback);
    void subscribeToOnMapChange(std::function<void(uint8_t&)> pCallback);
    void subscribeToLogout(std::function<void()> pCallback);

  private:
#ifdef DEBUG
  static std::string PACKET_NAMES[]; //!< Array of regular packet names
  static std::string EXTENDED_PACKET_NAMES[]; //!< Array of extended packet names
  static std::string ULTIMA_LIVE_PACKET_NAMES[]; //!< Array of UltimaLive packet names
#endif
    //ultima live events - for now there's a max of one subscriber
    std::vector<std::function<void(std::vector<MapDefinition>)>> m_onMapDefinitionUpdateSubscribers;     //!< MapDefinitionUpdate event subscriber list 
    std::vector<std::function<void(uint8_t, uint32_t, uint8_t*)>> m_onLandUpdateSubscriber;              //!< LandUpdate event subscriber list
    std::vector<std::function<void(uint8_t, uint32_t, uint8_t*, uint32_t)>> m_onStaticsUpdateSubscriber; //!< StaticsUpdate event subscriber list
    std::vector<std::function<void()>> m_onRefreshClientViewSubscriber;                                  //!< RefreshClientView event subscriber list
    std::vector<std::function<void(uint32_t, uint8_t)>> m_onBlockQueryRequestSubscriber;				 //!< BlockQueryRequest event subscriber list
    std::vector<std::function<void(std::string)>> m_onUltimaLiveLoginCompleteSubscriber;				 //!< UltimaLive LoginComplete event subscriber list

    //regular game logic
    std::vector<std::function<void()>> m_onServerMobileUpdateSubscribers;      //!< MobileUpdate event subscriber list
    std::vector<std::function<void(uint8_t*)>> m_onLoginConfirmSubscribers;    //!< LoginConfirm event subscriber list
    std::vector<std::function<void()>> m_onLoginCompleteSubscribers;		   //!< LoginComplete event subscriber list
    std::vector<std::function<void(uint8_t&)>> m_onBeforeMapChangeSubscribers; //!< BeforeMapChange event subscriber list
    std::vector<std::function<void(uint8_t&)>> m_onChangeMapSubscribers;	   //!< ChangeMap event subscriber list
    std::vector<std::function<void()>> m_onLogoutSubscribers;				   //!< Logout event subscriber list

    //packet handler maps
    std::map<uint8_t, BasePacketHandler*> m_ultimaLiveHandlers;         //!< UltimaLive Packet Handlers
    std::map<uint8_t, BasePacketHandler*> m_sendPacketHandlers;			//!< Client to Server Packet Handlers
    std::map<uint8_t, BasePacketHandler*> m_recvPacketHandlers;			//!< Server to Client Packet Handlers
    std::map<uint8_t, BasePacketHandler*> m_sendExtendedPacketHandlers;	//!< Client to Server Extended Packet Handlers
    std::map<uint8_t, BasePacketHandler*> m_recvExtendedPacketHandlers;	//!< Server to Client Extended Packet Handlers

    Client* m_pClient; //!< Local reference to the global client class instance

    //extended packet handler redirection and ultima live packet handler redirection
    bool OnReceiveServerUltimaLivePacket(unsigned char *pBuffer);
    bool OnReceiveExtendedPacket(unsigned char *pBuffer);
    bool OnSendExtendedPacket(unsigned char *pBuffer);

  #pragma region Self Registration
  public:
    static bool Initialize();
    static void Configure();

  private:
    static SelfRegisteringClass <NetworkManager> m_registration; //!< Self Registering Class mechanism
  #pragma endregion
};

#endif
