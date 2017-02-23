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


#include "UltimaLive.h"
#include "Client.h"

UltimaLive* UltimaLive::g_pUltimaLive = NULL;

/**
 * @brief UltimaLive class constructor
 */
UltimaLive::UltimaLive()
  : m_pClient(NULL),
    m_versionMajor(0),
    m_versionMinor(0),
    m_registrations()
{
  std::string currentModuleFullPath = Utils::GetCurrentModuleFullPath();
  uint32_t fileVersionSize = GetFileVersionInfoSizeA(currentModuleFullPath.c_str(), NULL);
  if (fileVersionSize > 0)
  {
    char* pVersionBuffer = new char[fileVersionSize];
    char* pQueryValueBuffer = NULL;

    if (GetFileVersionInfo(currentModuleFullPath.c_str(), NULL, fileVersionSize, pVersionBuffer))
    {
      uint32_t size = 0;
      if (VerQueryValue(pVersionBuffer, "\\", reinterpret_cast<VOID FAR* FAR*>(&pQueryValueBuffer), &size))
      {
        if (size > 0)
        {
          VS_FIXEDFILEINFO *verInfo = (VS_FIXEDFILEINFO *)pQueryValueBuffer;
          if (verInfo->dwSignature == 0xfeef04bd)
          {
              int major = HIWORD(verInfo->dwFileVersionMS);
              int minor = LOWORD(verInfo->dwFileVersionMS);
              int buildMajor = HIWORD(verInfo->dwFileVersionLS);
              int buildMinor = LOWORD(verInfo->dwFileVersionLS);

              char buffer[20];
              memset(buffer, 0x00, 20);
              sprintf_s(buffer, "%i.%i.%i.%i", major, minor, buildMajor, buildMinor);

              m_versionMajor = verInfo->dwFileVersionMS;
              m_versionMinor = verInfo->dwFileVersionLS;
          }
        }
      }
    }

    delete [] pVersionBuffer;
  }
}

/**
 * @brief Starts UltimaLive. Searches for internal client structures and prepares to start handling packets. 
 */
void UltimaLive::Start()
{
  #ifdef DEBUG
  Logger::g_pLogger->InitializeLogger();
  #endif

  Logger::g_pLogger->LogPrint("Welcome to UltimaLive!\n");
  m_pClient = new Client();
  BaseSelfRegisteringClass::Configure();

  bool safeToProceed = true;

  if (!BaseSelfRegisteringClass::Initialize())
  {
    safeToProceed = false;
    Logger::g_pLogger->LogPrintError("FAILED to initialize all UltimaLive registered classes.\n");
  }
  else
  {
    Logger::g_pLogger->LogPrint("All UltimaLive registerd classes successfully initialized.\n");
  }

  if (safeToProceed)
  {
    if (!ClientRedirections::InstallClientHooks())
    {
      #ifdef DEBUG
      system("pause");
      #else
      MessageBoxA(NULL,"Unable to install client hooks, exiting.", "Error", NULL);
      #endif
      exit(-1);
    }

    #if defined DEBUG && defined DOTNET
    Logger::g_pLogger->LogPrint("Launching .NET");
    g_pDotNetHost = new DotNetHost();
    g_pDotNetHost->LaunchCLR();
    uint32_t result = g_pDotNetHost->LaunchNetAssembly("UltimaLiveObjectViewer.dll", "UltimaLiveObjectViewer.ObjectViewer", "ShowMainWindow", "asdf");
    #endif
  }
  else // we failed to find all of the required client internal addresses
  {   
    Logger::g_pLogger->g_pLogger->LogPrintWarning("Reverting to regular client.\n");
    LocalPeHelper32 ultimaHelper(Utils::GetCurrentModuleName());   

    bool success = true;

    if (!ultimaHelper.Init())
    {
      success = false;
    }

    if (success)
    {
      if (!ultimaHelper.PatchImportedFunctionAddress("kernel32.dll", "CreateFileA", (DWORD)&CreateFileA))
      {
        success = false;
      }
    }

    if (!success)
    {

      Logger::g_pLogger->LogPrintError("Could not restore CreateFileA!\n");
      Logger::g_pLogger->LogPrintError("Cannot recover! Exiting!\n");
      #ifdef DEBUG
      system("pause");
      #else
      MessageBoxA(NULL,"Could not restore CreateFileA! Cannot recover! Exiting.", "Error", NULL);
      #endif
      exit(-1);
    }

    Logger::g_pLogger->LogPrintWarning("Restored original CreateFileA, continuing execution without UltimaLive.\n");
  }
}

/**
 * @brief Returns the UltimaLive major version
 *
 * @return Major version
 */
uint32_t UltimaLive::GetVersionMajor()
{
  return m_versionMajor;
}

/**
* @brief Returns the UltimaLive minor version
*
* @return Minor version
*/
uint32_t UltimaLive::GetVersionMinor()
{
  return m_versionMinor;
}

/**
* @brief Registers an instance of a class with a name.
*
* @return Major version
*/
void UltimaLive::Register(std::string name, void* obj)
{
  std::map<std::string, void*>::iterator it = m_registrations.find(name);

  if (it == m_registrations.end())
  {
    m_registrations[name] = obj;
  }
}

/**
 * @brief Looks up an instance of a class by name
 *
 * @param name lookup string
 *
 * @return Pointer to class instance
 */
void* UltimaLive::Lookup(std::string name)
{
  void* pResult = NULL;

  std::map<std::string, void*>::iterator it = m_registrations.find(name);

  if (it != m_registrations.end())
  {
    pResult = it->second;
  }

  return pResult;
}
