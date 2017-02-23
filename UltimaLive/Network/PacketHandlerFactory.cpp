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

#include "PacketHandlerFactory.h"
#include "ConcretePacketHandlers\ChangeMapHandler.h"
#include "ConcretePacketHandlers\LoginCompleteHandler.h"
#include "ConcretePacketHandlers\LogoutRequestHandler.h"
#include "ConcretePacketHandlers\LoginConfirmHandler.h"
#include "ConcretePacketHandlers\UltimaLiveRefreshClientViewHandler.h"
#include "ConcretePacketHandlers\UltimaLiveUpdateMapDefinitionsHandler.h"
#include "ConcretePacketHandlers\UltimaLiveUpdateStaticsHandler.h"
#include "ConcretePacketHandlers\UltimaLiveHashQueryHandler.h"
#include "ConcretePacketHandlers\UltimaLiveUpdateLandBlockHandler.h"
#include "ConcretePacketHandlers\UltimaLiveLoginCompleteHandler.h"
#include "ConcretePacketHandlers\ServerMobileStatusHandler.h"
#include "ConcretePacketHandlers\ServerVersionRequestHandler.h"
#include "ConcretePacketHandlers\ClientCrashPacketHandler.h"

/**
 * @brief Factory method that generates client to server packet handlers
 *
 * @param pManager Pointer to Network Manager Instance
 * @param versionMajor Client Major Version
 * @param versionMinor Client Minor Version
 * 
 * @return Map of packet IDs to packet handlers
 */
std::map<uint8_t, BasePacketHandler*> PacketHandlerFactory::GenerateClientPacketHandlers (uint32_t, uint32_t, NetworkManager* pManager)
{
  std::map<uint8_t, BasePacketHandler*> handlers;
  handlers[0x01] = new LogoutRequestHandler(pManager);

  #ifdef DEBUG
  handlers[0xF4] = new ClientCrashPacketHandler(pManager);
  #endif

  return handlers;
}

/**
 * @brief Factory method that generates UltimaLive server to client packet handlers
 *
 * @param versionMajor Client major version
 * @param versionMinor Client minor version
 * @param pManager Pointer to Network Manager Instance
 *
 * @return Map of packet IDs to packet handlers
 */
std::map<uint8_t, BasePacketHandler*> PacketHandlerFactory::GenerateUltimaLiveServerPacketHandlers (uint32_t, uint32_t, NetworkManager* pManager)
{
  std::map<uint8_t, BasePacketHandler*> handlers;
  handlers[0x00] = new UltimaLiveUpdateStaticsHandler(pManager);
  handlers[0x01] = new UltimaLiveUpdateMapDefinitionsHandler(pManager);
  handlers[0x02] = new UltimaLiveLoginCompleteHandler(pManager);
  handlers[0x03] = new UltimaLiveRefreshClientViewHandler(pManager);
  handlers[0xFF] = new UltimaLiveHashQueryHandler(pManager);

  return handlers;
}

/**
 * @brief Factory method that generates client to server extended packet handlers
 *
 * @param versionMajor Client major version
 * @param versionMinor Client minor version
 * @param pManager Pointer to Network Manager Instance
 *
 * @return Map of packet IDs to packet handlers
 */
std::map<uint8_t, BasePacketHandler*> PacketHandlerFactory::GenerateClientExtendedPacketHandlers (uint32_t, uint32_t, NetworkManager*)
{
  std::map<uint8_t, BasePacketHandler*> handlers;

  return handlers;
}

/**
 * @brief Factory method that generates server to client packet handlers
 *
 * @param versionMajor Client major version
 * @param versionMinor Client minor version
 * @param pManager Pointer to Network Manager Instance
 *
 * @return Map of packet IDs to packet handlers
 */
std::map<uint8_t, BasePacketHandler*> PacketHandlerFactory::GenerateServerPacketHandlers (uint32_t, uint32_t, NetworkManager* pManager)
{
  std::map<uint8_t, BasePacketHandler*> handlers;
  handlers[0x11] = new ServerMobileStatusHandler(pManager);
  handlers[0x1B] = new LoginConfirmHandler(pManager);
  handlers[0x40] = new UltimaLiveUpdateLandBlockHandler(pManager);
  handlers[0x55] = new LoginCompleteHandler(pManager);
  handlers[0xBD] = new ServerVersionRequestHandler(pManager);

  return handlers;
}

/**
 * @brief Factory method that generates server to client extended packet handlers
 *
 * @param versionMajor Client major version
 * @param versionMinor Client minor version
 * @param pManager Pointer to Network Manager Instance
 *
 * @return Map of packet IDs to packet handlers
 */
std::map<uint8_t, BasePacketHandler*> PacketHandlerFactory::GenerateServerExtendedPacketHandlers (uint32_t, uint32_t, NetworkManager* pManager)
{
  std::map<uint8_t, BasePacketHandler*> handlers;
  handlers[0x08] = new ChangeMapHandler(pManager);   

  return handlers;
}
