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

#include "ChangeMapHandler.h"
#include "..\NetworkManager.h"

/**
 * @brief ChangeMapHandler constructor
 */
ChangeMapHandler::ChangeMapHandler(NetworkManager* pManager)
  : BasePacketHandler(pManager)
{
  //do nothing
}

/**
 * @brief Handles the map change packet from the server 
 *
 * Packet Definition:
 *   byte 00            Packet ID: 0xBF (Extended Command)
 *   byte 01 - 02       Packet Size 
 *   byte 03 - 04       Extended Command ID: 0x08
 *   byte 05            Map ID
 *
 * @param pPacketData pointer to packet data
 *
 * @return True on success
 */
bool ChangeMapHandler::handlePacket(uint8_t* pPacketData)
{
  Logger::g_pLogger->LogPrint("Change MAP PACKET\n");
  m_pManager->onBeforeMapChange(pPacketData[5]);
  m_pManager->onMapChange(pPacketData[5]);

  return true;
}
