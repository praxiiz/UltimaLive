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

#include "FileManagerFactory.h"
#include "ConcreteFileManagers\FileManager_7_0_29_2.h"
#include "ConcreteFileManagers\FileManager.h"

#pragma region Self Registration
SelfRegisteringClass <FileManagerFactory> FileManagerFactory::m_registration;

/**
 * @brief Configure method for class registration. Creates the concrete factory class and adds it to the factory registry.
 */
void FileManagerFactory::Configure()
{
  Logger::g_pLogger->LogPrint("BaseFileManager configure\n");

  uint32_t versionMajor = UltimaLive::g_pUltimaLive->GetVersionMajor();
  uint32_t versionMinor = UltimaLive::g_pUltimaLive->GetVersionMinor();

  BaseFileManager* pFileManager = CreateFileManager(versionMajor, versionMinor);

  if (pFileManager != NULL)
  {
    UltimaLive::g_pUltimaLive->Register("FileManager", pFileManager);
  }
  else
  {
    Logger::g_pLogger->LogPrintError("FileManagerFactory failed to create an appropriate file manager for this client\n");
  }
}

/**
 * @brief Initialize method for class registration. Calls the concrete class's initialize method.
 * 
 * @return true on success
 */
bool FileManagerFactory::Initialize()
{
  BaseFileManager* pManager = static_cast<BaseFileManager*>(UltimaLive::g_pUltimaLive->Lookup("FileManager"));
  bool success = true;

  if (pManager != NULL)
  {
    Logger::g_pLogger->LogPrint("Initializing File Manager!\n"); 
    if (!pManager->Initialize())
    {
      success = false;
    }
  }
  else
  {
    success = false;
    Logger::g_pLogger->LogPrintError("Failed to Acquire FileManager Instance!\n"); 
  }

  return success;
}

#pragma endregion

/**
 * @brief Creates a concrete filemanager (packet factory) based on client version
 *
 * @param versionMajor Client Major Version 
 * @param versionMinor Client Minor Version
 * 
 * @return Pointer to the concrete filemanager
 */
BaseFileManager* FileManagerFactory::CreateFileManager(uint32_t versionMajor, uint32_t versionMinor)
{
  int a = (int)(versionMajor >> 16);
  int b = (int)(versionMajor & 0xFFFF);
  int c = (int)(versionMinor >> 16);
  int d = (int)(versionMinor & 0xFFFF);

  Logger::g_pLogger->LogPrint("version = %i, %i, %i, %i = 0x%x, 0x%x\n", a, b, c, d, versionMajor, versionMinor);
  BaseFileManager* pFileManager = NULL;

  //7.0.24.2 0007 0000 0018 0002

  if (versionMajor < 0x70000 || versionMinor < 0x180002)
  {
    pFileManager = new FileManager();
    Logger::g_pLogger->LogPrint("Using older client FileManager\n");
  }
  else
  {
    pFileManager = new FileManager_7_0_29_2();
    Logger::g_pLogger->LogPrint("Using FileManager 7.0.29.2\n");
  }

  return pFileManager;
}