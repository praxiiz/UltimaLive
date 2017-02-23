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


#include "ServerVersionRequestHandler.h"
#include "..\NetworkManager.h"

/**
 * @brief ServerVersionRequestHandler constructor
 */
ServerVersionRequestHandler::ServerVersionRequestHandler(NetworkManager* pManager)
  : BasePacketHandler(pManager)
{
  //do nothing
}

/**
 * @brief Sends the UltimaLive version to the server
 *
 * @return True
 */
bool ServerVersionRequestHandler::handlePacket(uint8_t*)
{
  uint16_t major = static_cast<uint16_t>(Utils::getModuleMinorVersionUpper());
  uint16_t minor = static_cast<uint16_t>(Utils::getModuleMinorVersionLower());

  //build packet
  uint8_t* pResponse = new uint8_t[21];
  pResponse[0] = 0x3F;                                               //byte 000              -  cmd
  *reinterpret_cast<uint16_t*>(pResponse + 1) = 21;                  //byte 001 through 002  -  packet size
  *reinterpret_cast<uint32_t*>(pResponse + 3) = 0x00000000;          //byte 003 through 006  -  no blocks associated with this packet
  *reinterpret_cast<uint32_t*>(pResponse + 7) = htonl(1);            //byte 007 through 010  -  number of statics in the packet (8 for a query response)          
  pResponse[11] = 0x00;                                              //byte 011              -  UltimaLive sequence number
  pResponse[12] = 0x00;                                              //byte 012              -  UltimaLive sequence number
  pResponse[13] = 0xFE;                                              //byte 013              -  UltimaLive command (0xFE is a send UltimaLive Version Number)
  pResponse[14] = 0;                                                 //byte 014              -  No Map Associated with this packet
  *reinterpret_cast<uint16_t*>(pResponse + 15) = htons(major);       //byte 015 through 016  -  Major Version Number  
  *reinterpret_cast<uint16_t*>(pResponse + 17) = htons(minor);       //byte 017 through 018  -  Minor Version Number
  pResponse[19] = 0;
  pResponse[20] = 0;

  Logger::g_pLogger->LogPrint("Sending UltimaLive Version to server");
  m_pManager->sendPacketToServer(pResponse);
  return true;
}
