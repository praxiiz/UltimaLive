/* Copyright(c) 2016 UltimaLive
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

#include "UoLiveAppState.h"

UoLiveAppState::UoLiveAppState()
  : m_pFileManager(NULL),
  m_pNetworkManager(NULL),
  m_versionMajor(0),
  m_versionMinor(0),
  m_pAtlas(NULL),
  m_playerLocation(),
  m_movementRequests(),
  m_pClientPlayerStructure(NULL),
  m_pMapDimensions(NULL),
  m_pLoginHandler(NULL)
{
  //do nothing
}

void UoLiveAppState::setClientPlayerStructure(uint8_t* pPlayer)
{
  m_pClientPlayerStructure = pPlayer;
}

PlayerLocation UoLiveAppState::getPlayerLocation()
{
  if (m_pClientPlayerStructure != NULL)
  {
    uint32_t* pStaticBasePointer = reinterpret_cast<uint32_t*>(m_pClientPlayerStructure);
    uint8_t* pPlayerStructure = reinterpret_cast<uint8_t*>(*pStaticBasePointer);

    PlayerLocation loc;
    loc.Facing = 0x00;
    loc.X=0;
    loc.Y=0;
    loc.Z=0;
    loc.X = *reinterpret_cast<uint16_t*>(pPlayerStructure + 0x24);
    loc.Y = *reinterpret_cast<uint16_t*>(pPlayerStructure + 0x26);
    loc.Z = *reinterpret_cast<uint16_t*>(pPlayerStructure + 0x28);
    loc.Facing = *reinterpret_cast<uint8_t*>(pPlayerStructure + 0x72);

    return loc;
  }

  return PlayerLocation();
}


void UoLiveAppState::Init()
{
#ifdef DEBUG
  printf("Initializing UO Live App State\n");
#endif
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
              sprintf(buffer, "%i.%i.%i.%i", major, minor, buildMajor, buildMinor);

              m_versionMajor = verInfo->dwFileVersionMS;
              m_versionMinor = verInfo->dwFileVersionLS;
          }
        }
      }
    }

    delete pVersionBuffer;
  }

  m_pFileManager = FileManagerFactory::CreateFileManager(m_versionMajor, m_versionMinor);
  m_pFileManager->Initialize();

  m_pNetworkManager = new NetworkManager(this);
  m_pNetworkManager->init(m_versionMajor, m_versionMinor);

  m_pLoginHandler = new LoginHandler(m_pNetworkManager);
  m_pLoginHandler->init();

  m_pAtlas = new Atlas(m_pFileManager, this, m_pNetworkManager);
  m_pAtlas->init();

  m_pMapDimensions = MasterControlUtils::GetMapDimensionAddress();

#ifdef DEBUG
  printf("MAP DIMENSION POINTER: 0x%x\n", m_pMapDimensions);
#endif
}

Atlas* UoLiveAppState::GetAtlas()
{
  return m_pAtlas;
}

BaseFileManager* UoLiveAppState::GetFileManager()
{
  return m_pFileManager;
}

NetworkManager* UoLiveAppState::GetNetworkManager()
{
  return m_pNetworkManager;
}