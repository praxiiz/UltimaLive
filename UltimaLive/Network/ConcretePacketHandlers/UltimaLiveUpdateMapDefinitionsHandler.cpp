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


#include "UltimaLiveUpdateMapDefinitionsHandler.h"
#include "..\NetworkManager.h"
#include "..\..\Maps\MapDefinition.h"

 /**
 * @brief UltimaLiveUpdateMapDefinitionsHandler constructor
 */
UltimaLiveUpdateMapDefinitionsHandler::UltimaLiveUpdateMapDefinitionsHandler(NetworkManager* pManager)
  : BasePacketHandler(pManager)
{
  //do nothing
}

/**
 * @brief Decommuates UltimaLive Update Map Definitions packet
 *
 * @param pPacketData Pointer to packet data
 *
 * @return false (Do not forward packet to client)
 */
bool UltimaLiveUpdateMapDefinitionsHandler::handlePacket(uint8_t* pPacketData)
{
  std::vector<MapDefinition> definitions;

  uint32_t count = ntohl(*reinterpret_cast<uint32_t*>(&pPacketData[7]));
  int newNumMaps = (count * 7) / 9;

  for (int i = 0; i < newNumMaps; i++)
  {
    int offset = 15 + (i * 9);
    uint8_t mapNumber = (uint8_t)pPacketData[offset]; //iteration byte 000         -  map file index number
    uint16_t width     = ntohs(*reinterpret_cast<uint16_t*>(&pPacketData[offset + 1]));   //iteration byte 001 to 002  -  map width
    uint16_t height    = ntohs(*reinterpret_cast<uint16_t*>(&pPacketData[offset + 3]));  //iteration byte 003 to 004  -  map height
    uint16_t wrapX     = ntohs(*reinterpret_cast<uint16_t*>(&pPacketData[offset + 5]));   //iteration byte 005 to 006  -  wrap around dimension X
    uint16_t wrapY     = ntohs(*reinterpret_cast<uint16_t*>(&pPacketData[offset + 7]));   //iteration byte 007 to 008  -  wrap around dimension Y

    if ((width > 0) && (height > 0) && (wrapX > 0) && (wrapY > 0))
    {
      MapDefinition def;
      def.mapNumber = mapNumber;
      def.mapWidthInTiles = width;
      def.mapHeightInTiles = height;
      def.mapWrapWidthInTiles = wrapX;
      def.mapWrapHeightInTiles = wrapY;
      definitions.push_back(def);
    }
    else
    {
      Logger::g_pLogger->LogPrint("Received bad definition for map %i\n", i);
    }
  }

  m_pManager->onMapDefinitionUpdate(definitions);
  return false;
}
