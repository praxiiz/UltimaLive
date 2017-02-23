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

#include "LoginConfirmHandler_7_0_29_2.h"
#include "..\NetworkManager.h"

 /**
 * @brief LoginConfirmHandler_7_0_29_2 constructor
 */
LoginConfirmHandler_7_0_29_2::LoginConfirmHandler_7_0_29_2(NetworkManager* pManager)
  : BasePacketHandler(pManager)
{
  //do nothing
}

/**
 * @brief Decommutates login confirmation packets
 *
 *   byte 00         cmd
 *   byte 01 - 04    serial
 *   byte 05 - 08    00 00 00 00
 *   byte 09 - 10    Body
 *   byte 11 - 12    X
 *   byte 13 - 14    Y
 *   byte 15 - 16    Z
 *   byte 17         dir
 *   byte 18         00
 *   byte 19 - 22    FF FF FF FF
 *   byte 23 - 26    00 00 00 00
 *   byte 27 - 28    width
 *   byte 29 - 30    height
 *   byte 31 - 36    00 00 00 00 00 00
 *
 * @param pPacketData pointer to packet data
 *
 * @return True
 */
bool LoginConfirmHandler_7_0_29_2::handlePacket(uint8_t* pPacketData)
{
  m_pManager->onLoginConfirm(pPacketData);
  return true;
}
