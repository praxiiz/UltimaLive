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

class LoginHandler
{
  public:
    LoginHandler();

  private:
    void onLogoutRequest();
    void onLoginConfirm(uint8_t* pPacketData);
    void onLoginComplete();
    void onBeforeMapChange(uint8_t& rMap);
    void onHashQuery(uint32_t blockNumber, uint8_t mapNumber);
    void onServerMobileUpdate();
    void onUpdateMapDefinitions(std::vector<MapDefinition> definitions);

    NetworkManager* m_pManager;
    bool m_needToSendCachedLoginPacket;
    bool m_firstHashQuery;
    uint8_t* m_pCachedLoginPacket;
    bool m_firstMobileUpdateFromServer;

  #pragma region Self Registration
  public:
    static bool Initialize();
    static void Configure();

  private:
    static SelfRegisteringClass <LoginHandler> m_registration;
  #pragma endregion
};

#endif
