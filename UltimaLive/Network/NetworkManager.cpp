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


#include "NetworkManager.h"
#include "..\UltimaLive.h"

#pragma region Self Registration
SelfRegisteringClass <NetworkManager> NetworkManager::m_registration;

/**
 * @brief Creates global instance of network manager and register it with UltimaLive
 */
void NetworkManager::Configure()
{
  Logger::g_pLogger->LogPrint("NetworkManager configure\n");
  UltimaLive::g_pUltimaLive->Register("NetworkManager", new NetworkManager());
}

/**
 * @brief Called by self registering class construct. Generate packets handlers using Packet Handler Factories
 *
 * @return True on success
 */
bool NetworkManager::Initialize()
{
  NetworkManager* pManager = static_cast<NetworkManager*>(UltimaLive::g_pUltimaLive->Lookup("NetworkManager"));
  bool success = true;

  if (pManager != NULL)
  {
    Client* pClient = static_cast<Client*>(UltimaLive::g_pUltimaLive->Lookup("Client"));

    if (pClient != NULL)
    {
      pManager->m_pClient = pClient;

      uint32_t versionMajor = UltimaLive::g_pUltimaLive->GetVersionMajor();
      uint32_t versionMinor = UltimaLive::g_pUltimaLive->GetVersionMinor();
      
      Logger::g_pLogger->LogPrint("Initializing Network Manager!\n");
      
      pManager->m_sendPacketHandlers = PacketHandlerFactory::GenerateClientPacketHandlers(versionMajor, versionMinor, pManager);
      Logger::g_pLogger->LogPrintWithoutDate("%-75s", "   Registering Send-Normal-Packet handlers");
      Logger::g_pLogger->LogPrintWithoutDate(" Found: %03i ", pManager->m_sendPacketHandlers.size());
      Logger::g_pLogger->LogPrintTaskStatusResult(true);
      
      pManager->m_recvPacketHandlers = PacketHandlerFactory::GenerateServerPacketHandlers(versionMajor, versionMinor, pManager);
      Logger::g_pLogger->LogPrintWithoutDate("%-75s", "   Registering Receive-Normal-Packet handlers");
      Logger::g_pLogger->LogPrintWithoutDate(" Found: %03i ", pManager->m_recvPacketHandlers.size());
      Logger::g_pLogger->LogPrintTaskStatusResult(true);
      
      pManager->m_sendExtendedPacketHandlers = PacketHandlerFactory::GenerateClientExtendedPacketHandlers(versionMajor, versionMinor, pManager);
      Logger::g_pLogger->LogPrintWithoutDate("%-75s", "   Registering Send-Extended-Packet handlers");
      Logger::g_pLogger->LogPrintWithoutDate(" Found: %03i ", pManager->m_sendExtendedPacketHandlers.size());
      Logger::g_pLogger->LogPrintTaskStatusResult(true);
      
      pManager->m_recvExtendedPacketHandlers = PacketHandlerFactory::GenerateServerExtendedPacketHandlers(versionMajor, versionMinor, pManager);
      Logger::g_pLogger->LogPrintWithoutDate("%-75s", "   Registering Receive-Extended-Packet handlers");
      Logger::g_pLogger->LogPrintWithoutDate(" Found: %03i ", pManager->m_recvExtendedPacketHandlers.size());
      Logger::g_pLogger->LogPrintTaskStatusResult(true);
      
      pManager->m_ultimaLiveHandlers = PacketHandlerFactory::GenerateUltimaLiveServerPacketHandlers(versionMajor, versionMinor, pManager);
      Logger::g_pLogger->LogPrintWithoutDate("%-75s", "   Registering Receive-UltimaLive-Packet handlers");
      Logger::g_pLogger->LogPrintWithoutDate(" Found: %03i ", pManager->m_ultimaLiveHandlers.size());
      Logger::g_pLogger->LogPrintTaskStatusResult(true);
    }
    else
    {
      Logger::g_pLogger->LogPrintError("NetworkManager: Failed to acquire Client class Instance\n");
      success = false;
    }
  }
  else
  {
    Logger::g_pLogger->LogPrintError("NetworkManager: Failed to acquire NetworkManager Instance\n");
    success = false;
  }

  return success;
}

#pragma endregion

/**
 * @brief Sends a packet to the client using the client class
 *
 * @param pBuffer Pointer to packet data
 */
void NetworkManager::sendPacketToClient(uint8_t* pBuffer)
{
  m_pClient->SendPacketToClient(pBuffer);
}

/**
 * @brief Sends a packet to the server using the client class
 *
 * @param pBuffer Pointer to packet data
 */
void NetworkManager::sendPacketToServer(uint8_t* pBuffer)
{
  m_pClient->SendPacketToServer(pBuffer);
}

/**
 * @brief NetworkManager constructor
 */
NetworkManager::NetworkManager()
  : m_onMapDefinitionUpdateSubscribers(),
  m_onLandUpdateSubscriber(),
  m_onStaticsUpdateSubscriber(),
  m_onRefreshClientViewSubscriber(),
  m_onBlockQueryRequestSubscriber(),
  m_onUltimaLiveLoginCompleteSubscriber(),
  m_onServerMobileUpdateSubscribers(),
  m_onLoginConfirmSubscribers(),
  m_onLoginCompleteSubscribers(),
  m_onBeforeMapChangeSubscribers(),
  m_onChangeMapSubscribers(),
  m_onLogoutSubscribers(),
  m_ultimaLiveHandlers(),
  m_sendPacketHandlers(),
  m_recvPacketHandlers(),
  m_sendExtendedPacketHandlers(),
  m_recvExtendedPacketHandlers(),
  m_pClient(NULL)
{
  //do nothing
}

/**
 * @brief Called to handle UltimaLive packets. Calls packet handlers in turn.
 *
 * @return False (does not forward packets to client)
 */
bool NetworkManager::OnReceiveServerUltimaLivePacket(unsigned char* pBuffer)
{
  bool retVal = false;
  uint8_t command = pBuffer[13];

#if defined(DEBUG) && defined(PRINT_PACKETS) 
  std::stringstream packetName;
  packetName << "Received SERVER_ULTIMALIVE_" << ULTIMA_LIVE_PACKET_NAMES[command];
  packetName << " (0x" << std::hex << (int)command << ")";
  Logger::g_pLogger->LogPacketToClient("%s\n",  packetName.str().c_str());
#endif

  if (m_ultimaLiveHandlers.find(command) != m_ultimaLiveHandlers.end())
  {
    m_ultimaLiveHandlers[command]->handlePacket(pBuffer);
  }

  return retVal;
}

/**
 * @brief Called to handle server packets. Calls packet handlers in turn.
 *
 * @param pBuffer Pointer to packet data
 *
 * @return Returns the return code from the called packet handler. If false, do not forward packets to client.
 */
bool NetworkManager::OnReceivePacket(unsigned char *pBuffer)
{
  bool retVal = true;
  uint8_t command = pBuffer[0];

  switch (command)
  {
    case 0x3F:
    {
      retVal = OnReceiveServerUltimaLivePacket(pBuffer);
    }
    break;

    case 0xBF:
    {
      retVal = OnReceiveExtendedPacket(pBuffer);
    }
    break;

    default:
    {
      if (m_recvPacketHandlers.find(command) != m_recvPacketHandlers.end())
      {
        retVal = m_recvPacketHandlers[command]->handlePacket(pBuffer);
      }
    }
    break;
  }

#if defined(DEBUG) && defined(PRINT_PACKETS)
  if (command != 0xBF && command != 0x3F && command != 0x22)
  {
    std::stringstream packetName;
    packetName << "Received SERVER_" << PACKET_NAMES[command];
    packetName << " (0x" << std::hex << (int)command << ")";  
    Logger::g_pLogger->LogPacketToClient("%s\n",  packetName.str().c_str());
  }
#endif

#if defined(DEBUG) && defined(PRINT_PACKET_HEX)
  LocalPeHelper32::HexPrint(pBuffer, 32);
#endif


  return retVal;
}

/**
 * @brief Calls packet handlers on an outgoing packet
 *
 * @param pBuffer Pointer to packet data
 *
 * @return true to forward the packet onto the server
 */
bool NetworkManager::OnSendPacket(unsigned char *pBuffer)
{
  bool retVal = true;
  uint8_t command = pBuffer[0];

  if (command == 0xBF)
  {
    retVal = OnSendExtendedPacket(pBuffer);
  }
  else 
  {
    if (command == 0xF4)
    {
      Logger::g_pLogger->LogPrint("CLIENT HAS CRASHED!   Crash Report:\n");
      LocalPeHelper32::HexPrint(pBuffer, *reinterpret_cast<uint16_t*>(&pBuffer[1]));
    }
    else if (m_sendPacketHandlers.find(command) != m_sendPacketHandlers.end())
    {
      retVal = m_sendPacketHandlers[command]->handlePacket(pBuffer);
    }

#if defined(DEBUG) && defined(PRINT_PACKETS)
    std::stringstream packetName;
    packetName << "Sent CLIENT_" << PACKET_NAMES[command];
    packetName << " (0x" << std::hex << (int)command << ")";  
    Logger::g_pLogger->LogPacketToServer("%s\n",  packetName.str().c_str());
#endif
  }

  return retVal;
}

/**
 * @brief Handles extended packets from the server
 *
 * @param pBuffer Pointer to packet data
 *
 * @return True to pass packet onto server
 */
bool NetworkManager::OnReceiveExtendedPacket(unsigned char *pBuffer)
{
  bool retVal = true;

  uint8_t command = static_cast<uint8_t>(ntohs(*reinterpret_cast<uint16_t*>(pBuffer + 3)));

  if (m_recvExtendedPacketHandlers.find(command) != m_recvExtendedPacketHandlers.end())
  {
    retVal = m_recvExtendedPacketHandlers[command]->handlePacket(pBuffer);
  }

#if defined(DEBUG) && defined(PRINT_PACKETS)
  std::stringstream packetName;
  packetName << "Received SERVER_EXTENDED_" << EXTENDED_PACKET_NAMES[command];
  packetName << " (0x" << std::hex << (int)command << ")";  
  Logger::g_pLogger->LogPacketToClient("%s\n",  packetName.str().c_str());
#endif

  return retVal;
}

/**
 * @brief Handles extended packets before they are sent to the server
 *
 * @param pBuffer Pointer to data packet
 *
 * @return True to pass data onto server
 */
bool NetworkManager::OnSendExtendedPacket(unsigned char *pBuffer)
{
  uint8_t command = static_cast<uint8_t>(ntohs(*reinterpret_cast<uint16_t*>(pBuffer + 3)));

  bool retVal = true;
  if (m_sendExtendedPacketHandlers.find(command) != m_sendExtendedPacketHandlers.end())
  {
    retVal = m_sendExtendedPacketHandlers[command]->handlePacket(pBuffer);
  }

#if defined(DEBUG) && defined(PRINT_PACKETS)
  std::stringstream packetName;
  packetName << "sent CLIENT_EXTENDED_" << EXTENDED_PACKET_NAMES[command];
  packetName << "_(0x" << std::hex << (int)command << ")";
  Logger::g_pLogger->LogPacketToServer("%s\n",  packetName.str().c_str());
#endif

  return retVal;
}

/**
 * @brief Subscribes to the UltimaLive update map definitions packet
 *
 * @param pCallback Packet handler function
 */
void NetworkManager::subscribeToMapDefinitionUpdate(std::function<void(std::vector<MapDefinition>)> pCallback)
{
  m_onMapDefinitionUpdateSubscribers.push_back(pCallback);
}

/**
 * @brief Subscribes to the UltimaLive update land packet
 *
 * @param pCallback Packet handler function
 */
void NetworkManager::subscribeToLandUpdate(std::function<void(uint8_t, uint32_t, uint8_t*)> pCallback)
{
  m_onLandUpdateSubscriber.push_back(pCallback);
}

/**
 * @brief Subscribes to the UltimaLive update statics packet
 *
 * @param pCallback Packet handler function
 */
void NetworkManager::subscribeToStaticsUpdate(std::function<void(uint8_t, uint32_t, uint8_t*, uint32_t)> pCallback)
{
  m_onStaticsUpdateSubscriber.push_back(pCallback);
}

/**
 * @brief Subscribes to the UltimaLive refresh client packet
 *
 * @param pCallback Packet handler function
 */
void NetworkManager::subscribeToRefreshClient(std::function<void()> pCallback)
{
  m_onRefreshClientViewSubscriber.push_back(pCallback);
}

/**
 * @brief Subscribes to the UltimaLive block query packet
 *
 * @param pCallback Packet handler function
 */
void NetworkManager::subscribeToBlockQueryRequest(std::function<void(int32_t, uint8_t)> pCallback)
{
  m_onBlockQueryRequestSubscriber.push_back(pCallback);
}

/**
 * @brief Subscribes to the UltimaLive login complete packet
 *
 * @param pCallback Packet handler function
 */
void NetworkManager::subscribeToUltimaLiveLoginComplete(std::function<void(std::string)> pCallback)
{
  m_onUltimaLiveLoginCompleteSubscriber.push_back(pCallback);
}

/**
 * @brief Subscribes to the Update Mobile packet
 *
 * @param pCallback Packet handler function
 */
void NetworkManager::subscribeToServerMobileUpdate(std::function<void()> pCallback)
{
  m_onServerMobileUpdateSubscribers.push_back(pCallback);
}

/**
 * @brief Subscribes to the Login Confirm packet
 *
 * @param pCallback Packet handler function
 */
void NetworkManager::subscribeToLoginConfirm(std::function<void(uint8_t*)> pCallback)
{
  m_onLoginConfirmSubscribers.push_back(pCallback);
}

/**
 * @brief Subscribes to the Login Complete packet
 *
 * @param pCallback Packet handler function
 */
void NetworkManager::subscribeToLoginComplete(std::function<void()> pCallback)
{
  m_onLoginCompleteSubscribers.push_back(pCallback);
}

/**
 * @brief Subscribes to on before Map Change packet event
 *
 * @param pCallback Packet handler function
 */
void NetworkManager::subscribeToOnBeforeMapChange(std::function<void(uint8_t&)> pCallback)
{
  m_onBeforeMapChangeSubscribers.push_back(pCallback);
}

/**
 * @brief Subscribes to the Map Change packet
 *
 * @param pCallback Packet handler function
 */
void NetworkManager::subscribeToOnMapChange(std::function<void(uint8_t&)> pCallback)
{
  m_onChangeMapSubscribers.push_back(pCallback);
}

/**
 * @brief Subscribes to the Logout packet
 *
 * @param pCallback Packet handler function
 */
void NetworkManager::subscribeToLogout(std::function<void()> pCallback)
{
  m_onLogoutSubscribers.push_back(pCallback);
}

/**
 * @brief Fires the update map definitions event
 *
 * @param definitions Map Definitions
 */
void NetworkManager::onMapDefinitionUpdate(std::vector<MapDefinition> definitions)
{
  for (std::vector<std::function<void(std::vector<MapDefinition>)>>::iterator itr = m_onMapDefinitionUpdateSubscribers.begin(); itr != m_onMapDefinitionUpdateSubscribers.end(); itr++)
  {
    (*itr)(definitions);
  }
}

/**
 * @brief Fires the update land event
 *
 * @param mapNumber Map Number
 * @param blockNumber Block Number
 * @param pLandData pointer to land data
 */
void NetworkManager::onLandUpdate(uint8_t mapNumber, uint32_t blockNumber, uint8_t* pLandData)
{
  for (std::vector<std::function<void(uint8_t, uint32_t, uint8_t*)>>::iterator itr = m_onLandUpdateSubscriber.begin(); itr != m_onLandUpdateSubscriber.end(); itr++)
  {
    (*itr)(mapNumber, blockNumber, pLandData);
  }
}

/**
 * @brief Fires the update statics event
 *
 * @param mapNumber Map Number
 * @param blockNumber Block Number
 * @param pStaticsData Pointer to statics data
 * @param length of statics data in bytes
 */
void NetworkManager::onStaticsUpdate(uint8_t mapNumber, uint32_t blockNumber, uint8_t* pStaticsData, uint32_t length)
{
  for (std::vector<std::function<void(uint8_t, uint32_t, uint8_t*, uint32_t)>>::iterator itr = m_onStaticsUpdateSubscriber.begin(); itr != m_onStaticsUpdateSubscriber.end(); itr++)
  {
    (*itr)(mapNumber, blockNumber, pStaticsData, length);
  }
}

/**
 * @brief Fires the refresh client event
 */
void NetworkManager::onRefreshClient()
{
  for(std::vector<std::function<void()>>::iterator itr = m_onRefreshClientViewSubscriber.begin(); itr != m_onRefreshClientViewSubscriber.end(); itr++)
  {
    (*itr)();
  }
}

/**
 * @brief Fires the block query request event
 *
 * @param blockNumber Block Number
 * @param mapNumber Map Number
 */
void NetworkManager::onBlockQueryRequest(int32_t blockNumber, uint8_t mapNumber)
{
  for (std::vector<std::function<void(uint32_t, uint8_t)>>::iterator itr = m_onBlockQueryRequestSubscriber.begin(); itr != m_onBlockQueryRequestSubscriber.end(); itr++)
  {
    (*itr)(blockNumber, mapNumber);
  }
}

/**
* @brief Fires the Ultaima Live Login Complete event
*
* @param shardIdentifier Unique Shard Identifier
*/
void NetworkManager::onUltimaLiveLoginComplete(std::string shardIdentifier)
{
  for (std::vector<std::function<void(std::string)>>::iterator itr = m_onUltimaLiveLoginCompleteSubscriber.begin(); itr != m_onUltimaLiveLoginCompleteSubscriber.end(); itr++)
  {
    (*itr)(shardIdentifier);
  }
}

/**
* @brief Fires the Server Mobile Update event
*/
void NetworkManager::onServerMobileUpdate()
{
  for (std::vector<std::function<void()>>::iterator itr = m_onServerMobileUpdateSubscribers.begin(); itr != m_onServerMobileUpdateSubscribers.end(); itr++)
  {
    (*itr)();
  }
}

/**
* @brief Fires the Login Confirm event
*
* @param pData Pointer packet data
*/
void NetworkManager::onLoginConfirm(uint8_t* pData)
{
  for(std::vector<std::function<void(uint8_t*)>>::iterator itr = m_onLoginConfirmSubscribers.begin(); itr != m_onLoginConfirmSubscribers.end(); itr++)
  {
    (*itr)(pData);
  }
}

/**
* @brief Fires the Login Complete event
*/
void NetworkManager::onLoginComplete()
{
  for (std::vector<std::function<void()>>::iterator itr = m_onLoginCompleteSubscribers.begin(); itr != m_onLoginCompleteSubscribers.end(); itr++)
  {
    (*itr)();
  }
}

/**
* @brief Fires the Before Map Change event
*
* @param mapNumber Map Number
*/
void NetworkManager::onBeforeMapChange(uint8_t& mapNumber) //return true to let specific handler code run
{
  for(std::vector<std::function<void(uint8_t&)>>::iterator itr = m_onBeforeMapChangeSubscribers.begin(); itr != m_onBeforeMapChangeSubscribers.end(); itr++)
  {
    (*itr)(mapNumber);
  }
}

/**
* @brief Fires the Map Change event
*
* @param mapNumber Map Number
*/
void NetworkManager::onMapChange(uint8_t& mapNumber) //return true to allow regular map change packet to go through
{
  for(std::vector<std::function<void(uint8_t&)>>::iterator itr = m_onChangeMapSubscribers.begin(); itr != m_onChangeMapSubscribers.end(); itr++)
  {
    (*itr)(mapNumber);
  }
}

/**
* @brief Fires the Log Out event
*/
void NetworkManager::onLogout()
{
  for(std::vector<std::function<void()>>::iterator itr = m_onLogoutSubscribers.begin(); itr != m_onLogoutSubscribers.end(); itr++)
  {
    (*itr)();
  }
}

#ifdef DEBUG
std::string NetworkManager::PACKET_NAMES[] =
{
  /* 0x00 - 0x07 */ "CHARACTER_CREATION",       "LOGOUT",                   "MOVEMENT_REQUEST",      "ASCII_SPEECH",             "TOGGLE_GOD_MODE",         "ATTACK_REQUEST",            "USE_REQUEST",                  "LIFT_REQUEST",
  /* 0x08 - 0x0F */ "DROP_REQUEST",             "LOOK_REQUEST",             "EDIT_ITEM_REQUEST",     "DAMAGE",                   "EDIT_TILE_DATA",          "EDIT_NPC_DATA",             "EDIT_TEMPLATE_DATA",           "PAPER_DOLL",
  /* 0x10 - 0x17 */ "EDIT_HUE_DATA",            "MOBILE_STATUS",            "TEXT_COMMAND",          "EQUIP_REQUEST",            "CHANGE_Z",                "FOLLOW_MESSAGE",            "UO_3D_MOBILE_STATUS_BAR",      "HEALTH_STATUS", 
  /* 0x18 - 0x1F */ "ADD_SCRIPT",               "EDIT_NPC_SPEECH",          "WORLD_ITEM",            "LOGIN_ACK",                "ASCII_MESSAGE",           "REMOVE_ENTITY",             "UNKNOWN_0X1E",                 "UNKNOWN_0X1F",
  /* 0x20 - 0x27 */ "MOBILE_UPDATE",            "MOVEMENT_REJECT",          "MOVEMENT_ACK",          "SERVER_DRAG_EFFECT",       "DISPLAY_GUMP",            "CONTAINER_CONTENT_UPDATE",  "GOD_CLIENT_KICK_CLIENT",       "SERVER_LIFT_REJECT",
  /* 0x28 - 0x2F */ "DROP_REJECT",              "DROP_ACCEPT",              "BLOOD_MODE",            "TOGGLE_GOD_MODE_RESPONSE", "DEATH_STATUS",            "MOBILE_ATTRIBUTES",         "EQUIP_UPDATE",                 "SWING",
  /* 0x30 - 0x37 */ "ATTACK_GRANTED",           "UO_3D_PET_WINDOW",         "TOGGLE_HACK_MOVER",     "GROUP_COMMAND",            "MOBILE_QUERY",            "RESOURCE_TYPE",             "RESOURCE_TILE_DATA",           "MOVE_OBJECT",
  /* 0x38 - 0x3F */ "PATH_FIND_MESSAGE",        "GROUPS",                   "SKILLS_UPDATE",         "END_VENDOR_TRANSACTION",   "CONTAINER_CONTENT",       "SHIP",                      "VERSIONS",                     "UPDATE_STATICS",
  /* 0x40 - 0x47 */ "UPDATE_TERRAIN",           "UPDATE_TERRAINS",          "UPDATE_ART",            "UPDATE_ANIMATIONS",        "HUES",                    "VERSION_OK",                "NEW_ART",                      "NEW_TERRAIN",
  /* 0x48 - 0x4F */ "NEW_ANIM_DATA",            "NEW_HUES",                 "DESTROY_ART",           "CHECK_VERSION",            "SCRIPT_NAMES",            "EDIT_SCRIPTS",              "PERSONAL_LIGHT_LEVEL",         "GLOBAL_LIGHT_LEVEL",
  /* 0x50 - 0x57 */ "UNKNOWN_0X50",             "UNKNOWN_0X51",             "UNKNOWN_0X52",          "POPUP_MESSAGE",            "PLAY_SOUND",              "LOGIN_COMPLETE",            "MAP_PLOT",                     "UPDATE_REGIONS",
  /* 0x58 - 0x5F */ "NEW_REGION",               "NEW_CONTENT_FX",           "UPDATE_CONTENT_FX",     "CURRENT_TIME",             "RESTART_VERSION",         "PLAY_CHARACTER",            "SERVERS_LIST",                 "ADD_SERVER",
  /* 0x60 - 0x67 */ "REMOVE_SERVER",            "DELETE_STATIC",            "MOVE_STATIC",           "AREA_LOAD",                "AREA_LOAD_REQUEST",       "WEATHER",                   "BOOK_PAGE",                    "SIMPED",
  /* 0x68 - 0x6F */ "SCRIPT_ATTACH",            "FRIENDS",                  "NOTIFY_FRIEND",         "KEY_USE",                  "TARGET",                  "PLAY_MUSIC",                "MOBILE_ANIMATION",             "SECURE_TRADE",
  /* 0x70 - 0x7F */ "GRAPHIC_EFFECT",           "BULLETIN_BOARD",           "SET_WAR_MODE",          "PING_ACK",                 "VENDOR_BUY_LIST",         "RENAME_REQUEST",            "CHANGE_SERVER",                "MOBILE_MOVING",
  /* 0x78 - 0x7F */ "MOBILE_INCOMING",          "RESOURCE_QUERY",           "RESOURCE_DATA",         "SEQUENCE",                 "DISPLAY_MENU",            "ITEM_LIST_MENU_RESPONSE",   "VIEW_QUERY",                   "VIEW_DATA",
  /* 0x80 - 0x87 */ "ACCOUNT_LOGIN_REQUEST",    "CHANGE_CHARACTER",         "ACCOUNT_LOGIN_REJ",     "DELETE_CHARACTER",         "UNKNOWN_0X84",            "DELETE_RESULT",             "CHARACTER_LIST_UPDATE",        "SEND_RESOURCES",
  /* 0x88 - 0x8F */ "DISPLAY_PAPERDOLL",        "CORPSE_EQUIP",             "TRIGGER_EDIT",          "DISPLAY_SIGN_GUMP",        "PLAY_SERVER_ACK",         "UO3D_CREATE_CHARACTER",     "MOVE_CHARACTER",               "UNKNOWN_0X8F",
  /* 0x90 - 0x97 */ "MAP_DETAILS",              "GAME_SERVER_LOGIN",        "UPDATE_MULTI_DATA",     "UNKNOWN_0X93",             "UPDATE_SKILLS",           "DISPLAY_HUE_PICKER",        "GAME_CENTRAL_MONITOR",         "PLAYER_MOVE",
  /* 0x98 - 0x9F */ "MOBILE_NAME_REQUEST",      "MULTI_TARGET_REQUEST",     "ASCII_PROMPT_RESPONSE", "HELP_GUMP_REQUEST",        "ASSISTANCE_REQUEST",      "GM_SINGLE",                 "VENDOR_SELL_LIST",             "VENDOR_SELL_REPLY",
  /* 0xA0 - 0xA7 */ "PLAY_SERVER",              "MOBILE_HITS",              "MOBILE_MANA",           "MOBILE_STAM",              "SYSTEM_INFO",             "LAUNCH_BROWSER",            "SCROLL_MESSAGE",               "REQUEST_SCROLL_WINDOW",
  /* 0xA8 - 0xAF */ "ACCOUNT_LOGIN_ACK",        "CHARACTER_LIST",           "CHANGE_COMBATANT",      "STRING_QUERY",             "STRING_RESPONSE",         "UNICODE_SPEECH",            "UNICODE_MESSAGE",              "DEATH_ANIMATION",
  /* 0xB0 - 0xB7 */ "DISPLAY_GENERIC_GUMP",     "GUMP_RESPONSE",            "CHAT_MESSAGE",          "CHAT_ACTION",              "TARGET_OBJECT_LIST",      "CHAT_OPEN_REQUEST",         "OBJECT_HELP_REQUEST",          "OBJECT_HELP_RESPONSE",
  /* 0xB8 - 0xBF */ "DISPLAY_PROFILE",          "SUPPORTED_FEATURES",       "CHANGE_ARROW",          "ACCOUNT_ID",               "SEASON_CHANGE",           "VERSION_REQUEST",           "ASSIST_VERSION_REQUEST",       "EXTENDED_COMMAND",
  /* 0xC0 - 0xC7 */ "HUED_EFFECT",              "MESSAGE_LOCALIZED",        "UNICODE_PROMPT",        "GQ_REQUEST",               "SEMI_VISIBLE",            "INVALID_MAP",               "INVALID_MAP_ENABLE",           "PARTICLE_EFFECT",
  /* 0xC8 - 0xCF */ "UPDATE_RANGE",             "TRIP_TIME_RESPONSE",       "U_TRIP_TIME_RESPONSE",  "GQ_COUNT",                 "MESSAGE_LOCALIZED_AFFIX", "UNKNOWN_0XCD",              "UNKNOWN_0XCE",                 "CLIENT_ACCOUNT_LOGIN",
  /* 0xD0 - 0xD7 */ "CONFIGURATION_FILE",       "LOGOUT_ACK",               "UNKNOWN_0XD2",          "UNKNOWN_0XD3",             "BOOK_HEADER",             "UNKNOWN_0XD5",              "BATCH_QUERY_PROPERTIES",       "ENCODED_COMMAND",
  /* 0xD8 - 0xDF */ "UNKNOWN_0xD8",             "HARDWARE_INFO",            "MAHJONG",               "CHARACTER_TRANSFER_LOG",   "OPL_INFO",                "DISPLAY_GUMP_PACKED",       "UPDATE_CHARACTER_COMBATANTS",  "BUFFS_AND_ATTRIBUTES",
  /* 0xE0 - 0xE7 */ "BUG_REPORT",               "UO3D_CLIENT_TYPE",         "NEW_MOBILE_ANIMATION",  "KR_ENCRYPTION_REQUEST",    "KR_ENCRYPTION_RESPONSE",  "UO_3D_DISPLAY_WAYPOINT",    "UO_3D_HIDE_WAYPOINT",          "UO_3D_CONTINUE_HIGHLIGHT_ELEMENT",
  /* 0xE8 - 0xEF */ "UO3D_REMOVE_HIGHLIGHT_UI", "UO_3D_TOG_HLT_ELEM",       "UO_3D_EN_HOTBAR",       "UO_3D_USE_HOTBAR_RESP",    "UO_3D_EQUIP_ITEMS_MACRO", "UO_3D_UNEQUIP_ITEMS_MACRO", "UNKNOWN_0XEE",                 "NEW_CLIENT_VERSION",           
  /* 0xF0 - 0xF7 */ "NEW_MOVEMENT_REQUEST",     "SERVER_TIME_SYNC_REQUEST", "TIME_SYNC_RESPONSE",    "NEW_WORLD_ITEM",           "CRASH_REPORT_PACKET",     "NEW_MAP_DETAILS",           "BOAT_MOVING_PACKET",           "PACKET_CONTAINER",             
  /* 0xF8 - 0xFF */ "NEW_CHARACTER_CREATION"
};

std::string NetworkManager::EXTENDED_PACKET_NAMES[] =
{
  /* 0x00 - 0x07 */ "UNKNOWN_00",               "FAST_WALK",                "ADD_WALK_KEY",           "UNKNOWN_03",                "CLOSE_GUMP",             "SCREEN_SIZE",              "PARTY_COMMAND",                  "QUEST_ARROW",
  /* 0x08 - 0x0F */ "MAP_CHANGE",               "DISARM_REQUEST",           "STUN_REQUEST",           "CLIENT_LANGUAGE",           "CLOSE_STATUS",           "UNKNOWN_0E",               "ANIMATE",                        "CLIENT_INFO",
  /* 0x10 - 0x17 */ "QUERY_OR_INFO",            "UNKNOWN_11",               "UNKNOWN_12",             "CONTEXT_MENU_REQUEST",      "DISPLAY_CONTEXT_MENU",   "CONTEXT_MENU_REQUEST",     "CLOSE_INTERFACE_WINDOW",         "DISPLAY_HELP_TOPIC",
  /* 0x19 - 0x1F */ "MAP_PATCHES",              "MISC_STATUS",              "STATUS_LOCK_CHANGE",     "SPELLBOOK_CONTENT",         "CAST_SPELL",             "DESIGN_HOUSE",             "QUERY_DESIGN_DETAILS",           "UNKNOWN_1F",
  /* 0x20 - 0x27 */ "HOUSE_CUSTOMIZATION",      "CLEAR_WEAPON_ABILITY",     "DAMAGE_PACKET",          "UNKNOWN_23",                "UNKNOWN_24",             "TOGGLE_SPECIAL_MOVES",     "MOVEMENT_SPEED_MODE",            "UNKNOWN_27",
  /* 0x29 - 0x2F */ "UNKNOWN_28",               "UNKNOWN_29",               "CHANGE_RACE",            "SET_MOBILE_ANIMATION",      "USE_TARGETED_ITEM",      "CAST_TARGETED_SPELL",      "USE_TARGETED_SKILL",             "KR_HOUSE_MENU_RESPONSE",
  /* 0x30 - 0x37 */ "UO3D_TARGET_BY_RESOURCE",  "UO3C_START_HBAR_SLOT_TMR", "TOGGLE_GARG_FLY",        "WHEEL_BOAT_MOVING",         "UNKNOWN_34",             "UNKNOWN_35",               "UNKNOWN_36",                     "UNKNOWN_37"
};


std::string NetworkManager::ULTIMA_LIVE_PACKET_NAMES[] =
{
  /* 0x00 - 0x07 */ "STATICS_UPDATE", "UPDATE_MAP_DEFINITIONS",   "LOGIN_CONFIRMATION",   "REFRESH_CLIENT",   "",   "",   "",
  /* 0x08 - 0x0F */ "",   "",   "",   "",   "",   "",   "",   "",   "",
  /* 0x10 - 0x17 */ "",   "",   "",   "",   "",   "",   "",   "",   "",
  /* 0x18 - 0x1F */ "",   "",   "",   "",   "",   "",   "",   "",   "",
  /* 0x20 - 0x27 */ "",   "",   "",   "",   "",   "",   "",   "",   "",
  /* 0x28 - 0x2F */ "",   "",   "",   "",   "",   "",   "",   "",   "",
  /* 0x30 - 0x37 */ "",   "",   "",   "",   "",   "",   "",   "",   "",
  /* 0x38 - 0x3F */ "",   "",   "",   "",   "",   "",   "",   "",   "",
  /* 0x40 - 0x47 */ "",   "",   "",   "",   "",   "",   "",   "",   "",
  /* 0x48 - 0x4F */ "",   "",   "",   "",   "",   "",   "",   "",   "",
  /* 0x50 - 0x57 */ "",   "",   "",   "",   "",   "",   "",   "",   "",
  /* 0x58 - 0x5F */ "",   "",   "",   "",   "",   "",   "",   "",   "",
  /* 0x60 - 0x67 */ "",   "",   "",   "",   "",   "",   "",   "",   "",
  /* 0x68 - 0x6F */ "",   "",   "",   "",   "",   "",   "",   "",   "",
  /* 0x70 - 0x7F */ "",   "",   "",   "",   "",   "",   "",   "",   "",
  /* 0x78 - 0x7F */ "",   "",   "",   "",   "",   "",   "",   "",   "",
  /* 0x80 - 0x87 */ "",   "",   "",   "",   "",   "",   "",   "",   "",
  /* 0x88 - 0x8F */ "",   "",   "",   "",   "",   "",   "",   "",   "",
  /* 0x90 - 0x97 */ "",   "",   "",   "",   "",   "",   "",   "",   "",
  /* 0x98 - 0x9F */ "",   "",   "",   "",   "",   "",   "",   "",   "",
  /* 0xA0 - 0xA7 */ "",   "",   "",   "",   "",   "",   "",   "",   "",
  /* 0xA8 - 0xAF */ "",   "",   "",   "",   "",   "",   "",   "",   "",
  /* 0xB0 - 0xB7 */ "",   "",   "",   "",   "",   "",   "",   "",   "",
  /* 0xB8 - 0xBF */ "",   "",   "",   "",   "",   "",   "",   "",   "",
  /* 0xC0 - 0xC7 */ "",   "",   "",   "",   "",   "",   "",   "",   "",
  /* 0xC8 - 0xCF */ "",   "",   "",   "",   "",   "",   "",   "",   "",
  /* 0xD0 - 0xD7 */ "",   "",   "",   "",   "",   "",   "",   "",   "",
  /* 0xD8 - 0xDF */ "",   "",   "",   "",   "",   "",   "",   "",   "",
  /* 0xE0 - 0xE7 */ "",   "",   "",   "",   "",   "",   "",   "",   "",
  /* 0xE8 - 0xEF */ "", 	"",   "",   "",   "",   "",   "",   "",   "",
  /* 0xF0 - 0xF7 */ "",   "",   "",   "",   "",   "",   "",   "",   "",
  /* 0xF8 - 0xFF */ "",   "",   "",   "",   "",   "",   "",   "",   "BLOCK_QUERY"
};  
#endif

