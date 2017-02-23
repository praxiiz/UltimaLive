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

#ifndef _DOT_NET_HOST_H
#define _DOT_NET_HOST_H

#include <string>
#include <stdint.h>
#include <Windows.h>
#include "Utils.h"
#include "Debug.h"

#include <metahost.h>
#pragma comment(lib, "mscoree.lib")

/**
 * @class DotNetHost
 * 
 * @brief Class used to host the .NET common language runtime (CLR) 
 */
class DotNetHost
{
  public:
    void LaunchCLR();
    uint32_t LaunchNetAssembly(std::string assemblyPath, std::string typeName, std::string methodName, std::string argument);
    DotNetHost();

  private:
    ICLRMetaHost* m_pMetaHost;          //!< Provides methods that return a specific version of the common language runtime (CLR) based on its version number.
    ICLRRuntimeInfo* m_pRuntimeInfo;    //!< Provides methods that return information about a specific common language runtime.
    ICLRRuntimeHost* m_pClrRuntimeHost; //!< Provides methods that enable the host to start and stop the common language runtime (CLR) explicitly.
    HRESULT m_hr;                       //!< Stores the result of launching the common language runtime (CLR)
};
#endif