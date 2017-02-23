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


#ifndef _BASE_PACKET_HANDLER_H
#define _BASE_PACKET_HANDLER_H

#include <stdint.h>

class NetworkManager;

/**
 * @class BasePacketHandler
 *
 * @brief Base class for all packet handlers
 */
class BasePacketHandler
{
  public:
    BasePacketHandler(NetworkManager* pManager);

	/**
	 * @brief Abstract virtual packet handler 
	 *
	 * @param pPacketData Pointer to packet data
	 */
    virtual bool handlePacket(uint8_t* pPacketData) = 0;

  protected:
    NetworkManager* m_pManager; //!< Pointer to the Network Manager Instance
};
#endif