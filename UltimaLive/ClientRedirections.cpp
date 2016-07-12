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

#include "ClientRedirections.h"

bool ClientRedirections::InstallClientHooks()
{
  bool success = true;

  Logger::g_pLogger->LogPrint("Proceeding to install UltimaLive client hooks...\n");
  Logger::g_pLogger->LogPrintWithoutDate("%-87s", "   Initializing function redirector");

  LocalPeHelper32 ultimaHelper(Utils::GetCurrentModuleName());    
  if (ultimaHelper.Init())
  {
    Logger::g_pLogger->LogPrintTaskStatusResult(true);
  }
  else
  {
    Logger::g_pLogger->LogPrintTaskStatusResult(false);
    Logger::g_pLogger->LogPrint("Unable to initialize function redirector! Fatal!");
    success = false;
  }

  if (success)
  {
    //remap CreateFileA to avoid calling this multiple times
    Logger::g_pLogger->LogPrintWithoutDate("%-87s", "   Redirecting CreateFileA");
    if (!ultimaHelper.PatchImportedFunctionAddress("kernel32.dll", "CreateFileA", (DWORD)&ClientRedirections::OnCreateFileA))
    {
      Logger::g_pLogger->LogPrintTaskStatusResult(false);
      Logger::g_pLogger->LogPrint("Failed to hook OnCreateFileA\n");
    }
    else
    {
      Logger::g_pLogger->LogPrintTaskStatusResult(true);
    }

    Logger::g_pLogger->LogPrintWithoutDate("%-87s", "   Redirecting MessageBoxA");
    if (!ultimaHelper.PatchImportedFunctionAddress("user32.dll", "MessageBoxA", (DWORD)&ClientRedirections::OnMessageBox))
    {
      Logger::g_pLogger->LogPrintTaskStatusResult(false);
      Logger::g_pLogger->LogPrint("Failed to hook MessageBox\n");
    }
    else
    {
      Logger::g_pLogger->LogPrintTaskStatusResult(true);
    }

    Logger::g_pLogger->LogPrintWithoutDate("%-87s", "   Redirecting CreateFileMappingA");
    if (!ultimaHelper.PatchImportedFunctionAddress("kernel32.dll", "CreateFileMappingA", (DWORD)&ClientRedirections::OnCreateFileMappingA))
    {
      Logger::g_pLogger->LogPrintTaskStatusResult(false);
      Logger::g_pLogger->LogPrint("Failed to hook CreateFileMappingA\n");
    }
    else
    {
      Logger::g_pLogger->LogPrintTaskStatusResult(true);
    }

    Logger::g_pLogger->LogPrintWithoutDate("%-87s", "   Redirecting CloseHandle");
    if (!ultimaHelper.PatchImportedFunctionAddress("kernel32.dll", "CloseHandle", (DWORD)&ClientRedirections::OnCloseHandle))
    {
      Logger::g_pLogger->LogPrintTaskStatusResult(false);
      Logger::g_pLogger->LogPrint("Failed to hook CloseHandle\n");
    }
    else
    {
      Logger::g_pLogger->LogPrintTaskStatusResult(true);
    }

    Logger::g_pLogger->LogPrintWithoutDate("%-87s", "   Redirecting MapViewOfFile");
    if (!ultimaHelper.PatchImportedFunctionAddress("kernel32.dll", "MapViewOfFile", (DWORD)&ClientRedirections::OnMapViewOfFile))
    {
      Logger::g_pLogger->LogPrintTaskStatusResult(false);
      Logger::g_pLogger->LogPrint("Failed to hook CreateFileMappingA\n");
    }
    else
    {
      Logger::g_pLogger->LogPrintTaskStatusResult(true);
    }

    Logger::g_pLogger->LogPrintWithoutDate("%-87s", "   Hooking client send function");
    BOOL bHook = Mhook_SetHook((PVOID*)&UltimaLive::g_pUltimaLive->m_pClient->m_pSendFunction, ClientRedirections::OnSendPacket);
    if (!bHook)
    {
      Logger::g_pLogger->LogPrintTaskStatusResult(false);
      success = false;
    }
    else
    {
      Logger::g_pLogger->LogPrintTaskStatusResult(true);
    }

    Logger::g_pLogger->LogPrintWithoutDate("%-87s", "   Hooking client receive function");
    bHook = Mhook_SetHook((PVOID*)&UltimaLive::g_pUltimaLive->m_pClient->m_pRecvFunction, ClientRedirections::OnReceivePacket);
    if (!bHook)
    {
      Logger::g_pLogger->LogPrintTaskStatusResult(false);
      success = false;
    }
    else
    {
      Logger::g_pLogger->LogPrintTaskStatusResult(true);
    }

    Logger::g_pLogger->LogPrintWithoutDate("%-87s", "   Hooking client update blocks function");
    bHook = Mhook_SetHook((PVOID*)&UltimaLive::g_pUltimaLive->m_pClient->m_pUpdateBlocksFunction, ClientRedirections::OnUpdateStaticBlocks);
    if (!bHook)
    {
      Logger::g_pLogger->LogPrintTaskStatusResult(false);
      success = false;
    }
    else
    {
      Logger::g_pLogger->LogPrintTaskStatusResult(true);
    }
  }
  return success;
}

HANDLE ClientRedirections::g_UpdateStaticBlocksMutex = CreateMutex(NULL, false, NULL);

void __fastcall ClientRedirections::OnReceivePacket(void* This, void*, unsigned char* pBuffer)
{
  if (UltimaLive::g_pUltimaLive->m_pClient->GetNetworkManager()->OnReceivePacket(pBuffer) == true)
  {
    UltimaLive::g_pUltimaLive->m_pClient->getRecvFunction()(This, pBuffer);
  }
}

int __fastcall ClientRedirections::OnSendPacket(void* This, void*, unsigned char* pBuffer)
{
  if (UltimaLive::g_pUltimaLive->m_pClient->GetNetworkManager()->OnSendPacket(pBuffer) == true)
  {
    int retValue = UltimaLive::g_pUltimaLive->m_pClient->getSendFunction()(This, pBuffer);

    return retValue;
  }

  return 0;
}

void ClientRedirections::OnUpdateStaticBlocks()
{
  WaitForSingleObject(g_UpdateStaticBlocksMutex, INFINITE); 
  UltimaLive::g_pUltimaLive->m_pClient->getUpdatesStaticBlocksFunction()();
  ReleaseMutex(g_UpdateStaticBlocksMutex);
}

HANDLE WINAPI ClientRedirections::OnCreateFileARunOnce(
  __in      LPCSTR lpFileName,
  __in      DWORD dwDesiredAccess,
  __in      DWORD dwShareMode,
  __in_opt  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
  __in      DWORD dwCreationDisposition,
  __in      DWORD dwFlagsAndAttributes,
  __in_opt  HANDLE hTemplateFile
  )
{
  UltimaLive::g_pUltimaLive = new UltimaLive();
  UltimaLive::g_pUltimaLive->Start();

  BaseFileManager* pManager = UltimaLive::g_pUltimaLive->m_pClient->GetFileManager();
  if (pManager != NULL)
  {
    return pManager->OnCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
  }

  return CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

HANDLE WINAPI ClientRedirections::OnCreateFileA(
  __in      LPCSTR lpFileName,
  __in      DWORD dwDesiredAccess,
  __in      DWORD dwShareMode,
  __in_opt  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
  __in      DWORD dwCreationDisposition,
  __in      DWORD dwFlagsAndAttributes,
  __in_opt  HANDLE hTemplateFile
  )
{
  BaseFileManager* pManager = UltimaLive::g_pUltimaLive->m_pClient->GetFileManager();
  if (pManager != NULL)
  {
    return UltimaLive::g_pUltimaLive->m_pClient->GetFileManager()->OnCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
  }

  return CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

LPVOID WINAPI ClientRedirections::OnMapViewOfFile(
  __in  HANDLE hFileMappingObject,
  __in  DWORD dwDesiredAccess,
  __in  DWORD dwFileOffsetHigh,
  __in  DWORD dwFileOffsetLow,
  __in  SIZE_T dwNumberOfBytesToMap
  )
{
  BaseFileManager* pManager = UltimaLive::g_pUltimaLive->m_pClient->GetFileManager();

  if (pManager != NULL)
  {
    return UltimaLive::g_pUltimaLive->m_pClient->GetFileManager()->OnMapViewOfFile(hFileMappingObject, dwDesiredAccess, dwFileOffsetHigh, dwFileOffsetLow, dwNumberOfBytesToMap);
  }

  return MapViewOfFile(hFileMappingObject, dwDesiredAccess, dwFileOffsetHigh, dwFileOffsetLow, dwNumberOfBytesToMap);
}

int WINAPI ClientRedirections::OnMessageBox(
  _In_opt_  HWND,
  _In_opt_  LPCTSTR lpText,
  _In_opt_  LPCTSTR lpCaption,
  _In_      UINT
  )
{
  Logger::g_pLogger->LogPrint("========================================================================================================\n");
  Logger::g_pLogger->LogPrint("MessageBox: %s \n", lpCaption);
  Logger::g_pLogger->LogPrint("========================================================================================================\n");
  Logger::g_pLogger->LogPrint("%s\n", lpText);
  Logger::g_pLogger->LogPrint("========================================================================================================\n");

  return IDOK;
}

HANDLE WINAPI ClientRedirections::OnCreateFileMappingA(
  __in      HANDLE hFile,
  __in_opt  LPSECURITY_ATTRIBUTES lpAttributes,
  __in      DWORD flProtect,
  __in      DWORD dwMaximumSizeHigh,
  __in      DWORD dwMaximumSizeLow,
  __in_opt  LPCSTR lpName
  )
{
  BaseFileManager* pManager = UltimaLive::g_pUltimaLive->m_pClient->GetFileManager();

  if (pManager != NULL)
  {
    return UltimaLive::g_pUltimaLive->m_pClient->GetFileManager()->OnCreateFileMappingA(hFile, lpAttributes, flProtect, dwMaximumSizeHigh, dwMaximumSizeLow, lpName);
  }

  return CreateFileMappingA(hFile, lpAttributes, flProtect, dwMaximumSizeHigh, dwMaximumSizeLow, lpName);
}


BOOL WINAPI ClientRedirections::OnCloseHandle(_In_  HANDLE hObject)
{
  BaseFileManager* pManager = UltimaLive::g_pUltimaLive->m_pClient->GetFileManager();
  if (pManager != NULL)
  {
    pManager->OnCloseHandle(hObject);
  }

  return CloseHandle(hObject);
}

