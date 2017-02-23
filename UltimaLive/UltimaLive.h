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


#ifndef _ULTIMA_LIVE_H
#define _ULTIMA_LIVE_H

#include <stdint.h>
#include <string>
#include <map>
#include <Windows.h>

#include "ClientRedirections.h"
#include "Utils.h"
#include "Debug.h"

class Client; 

/**
 * @class UltimaLive
 *
 * @brief Enables map streaming to the client and definition and use of additional maps
 */
class UltimaLive
{
  public:
    static UltimaLive* g_pUltimaLive; //!< Global class instance of UltimaLive 

    UltimaLive();

    void Start();
    void Register(std::string name, void* obj);
    void* Lookup(std::string name);

    uint32_t GetVersionMajor();
    uint32_t GetVersionMinor();
    Client* m_pClient; //!< Pointer to Client class instance

  private:
    uint32_t m_versionMajor; //!< UltimaLive Major Version
    uint32_t m_versionMinor; //!< UltimaLive Minor Version
    std::map<std::string, void*> m_registrations; //!< Class instance registrations
};

#endif