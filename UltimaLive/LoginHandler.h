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


#ifndef _LOGIN_HANDLER_H
#define _LOGIN_HANDLER_H

#include <stdint.h>
#include <string>
#include <vector>
#include "Utils.h"
#include "LocalPeHelper32.hpp"
#include "Debug.h"
#include "ClassRegistration\SelfRegisteringClass.h"

class NetworkManager;
class MapDefinition;

/**
 * @class LoginHandler
 *
 * @brief Handles the Ultima Client's Login 
 */
class LoginHandler
{
  public:
    LoginHandler();

  private:
    void onLogoutRequest();
    void onLoginConfirm(uint8_t* pPacketData);
    void onLoginComplete();
    void onBeforeMapChange(uint8_t& rMap);
    //void onHashQuery(uint32_t blockNumber, uint8_t mapNumber);
    void onServerMobileUpdate();
    void onUpdateMapDefinitions(std::vector<MapDefinition> definitions);

    NetworkManager* m_pManager;         //!< Pointer to the concrete network manager
    bool m_needToSendCachedLoginPacket; //!< Flag indicating whether or not to send a cached login packet as part of the login process
    bool m_firstHashQuery;              //!< Flag indicating if the first hash query has been made
    uint8_t* m_pCachedLoginPacket;      //!< Pointer to the cached login packet
    bool m_firstMobileUpdateFromServer; //!< Flag indicating if the first mobile update has been sent from the server

  #pragma region Self Registration
  public:
    static bool Initialize();
    static void Configure();

  private:
    static SelfRegisteringClass <LoginHandler> m_registration; //!< Self registering class variable which triggers the self registration
  #pragma endregion
};

#endif
