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


#include "UltimaLiveUpdateStaticsHandler.h"
#include "..\NetworkManager.h"

 /**
 * @brief UltimaLiveUpdateStaticsHandler constructor
 */
UltimaLiveUpdateStaticsHandler::UltimaLiveUpdateStaticsHandler(NetworkManager* pManager)
  : BasePacketHandler(pManager)
{
  //do nothing
}

/**
 * @brief Decommutate UltimaLive update statics packet and trigger OnStaticsUpdate event
 *
 * @param pPacketData Pointer to packet data
 *
 * @return false (Do not forward packet to client)
 */
bool UltimaLiveUpdateStaticsHandler::handlePacket(uint8_t* pPacketData)
{
  uint32_t blockNum = ntohl(*reinterpret_cast<uint32_t*>(&pPacketData[3]));
  uint32_t numStatics = ntohl(*reinterpret_cast<uint32_t*>(&pPacketData[7]));
  uint32_t totalBytes = numStatics * 7;
  uint8_t mapNumber = pPacketData[14];

  m_pManager->onStaticsUpdate(mapNumber, blockNum, &pPacketData[15], totalBytes);

  return false;
}
