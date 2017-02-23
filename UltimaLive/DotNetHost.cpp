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

#include "DotNetHost.h"

/**
 * @brief DotNetHost constructor 
 */
DotNetHost::DotNetHost()
  : m_pMetaHost(NULL),
    m_pRuntimeInfo(NULL),
    m_pClrRuntimeHost(NULL),
    m_hr(NULL)
{
  //do nothing
}

/**
 * @brief Sets up the common language runtime in the client process
 */
void DotNetHost::LaunchCLR()
{
  m_hr = CLRCreateInstance(CLSID_CLRMetaHost, IID_PPV_ARGS(&m_pMetaHost));
  m_hr = m_pMetaHost->GetRuntime(L"v4.0.30319", IID_PPV_ARGS(&m_pRuntimeInfo));
  m_hr = m_pRuntimeInfo->GetInterface(CLSID_CLRRuntimeHost, IID_PPV_ARGS(&m_pClrRuntimeHost));
  m_hr = m_pClrRuntimeHost->Start();
}

/** 
 * @brief Starts executing a common language runtime assembly inside the Client Process host
 */
uint32_t DotNetHost::LaunchNetAssembly(std::string assemblyPath, std::string typeName, std::string methodName, std::string argument)
{
  DWORD returnValue = 0;

  m_hr = m_pClrRuntimeHost->ExecuteInDefaultAppDomain(Utils::s2ws(assemblyPath.c_str()).c_str(), Utils::s2ws(typeName.c_str()).c_str(), Utils::s2ws(methodName.c_str()).c_str(), Utils::s2ws(argument.c_str()).c_str(), &returnValue);

  return returnValue;
}
