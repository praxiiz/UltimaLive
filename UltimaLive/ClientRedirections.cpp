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


#include "ClientRedirections.h"

/**
 * @brief Installs function hooks into the client to redirect client functions to ClientRedirections functions
 * 
 * @return true if successful
 */
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

/**
 * @brief This function is called in place of the client's network receive function. It  
 * passes control to the UltimaLive network manager which gives its subscribers the chance
 * to alter/process each network packet before the client.  It then calls the clients 
 * receive function.
 *
 * @param This Instance pointer
 * @param edx unknown
 * @param pBuffer Pointer to packet data
 */
void __fastcall ClientRedirections::OnReceivePacket(void* This, void*, unsigned char* pBuffer)
{
  if (UltimaLive::g_pUltimaLive->m_pClient->GetNetworkManager()->OnReceivePacket(pBuffer) == true)
  {
    UltimaLive::g_pUltimaLive->m_pClient->getRecvFunction()(This, pBuffer);
  }
}

/**
 * @brief This function is called in place of the client's network send function. It
 * passes control to the UltimaLive network manager which gives its subscribers the chance
 * to alter/process each network packet before they are sent to the server.  It then calls 
 * the clients receive function.
 * 
 * @param This Pointer to class instance
 * @param edx unknown
 * @param pBuffer Pointer to packet data
 *
 * @return 0
 */
int __fastcall ClientRedirections::OnSendPacket(void* This, void*, unsigned char* pBuffer)
{
  if (UltimaLive::g_pUltimaLive->m_pClient->GetNetworkManager()->OnSendPacket(pBuffer) == true)
  {
    int retValue = UltimaLive::g_pUltimaLive->m_pClient->getSendFunction()(This, pBuffer);

    return retValue;
  }

  return 0;
}

/**
 * @brief This function adds a mutex around the updates static blocks function in the
 * client to avoid issues caused by the client calling this function at the same time
 * as UltimaLive.
 */
void ClientRedirections::OnUpdateStaticBlocks()
{
  WaitForSingleObject(g_UpdateStaticBlocksMutex, INFINITE); 
  UltimaLive::g_pUltimaLive->m_pClient->getUpdatesStaticBlocksFunction()();
  ReleaseMutex(g_UpdateStaticBlocksMutex);
}

/**
 * @brief This function is hooked into the client by the Igrping.dll's main function.  It  
 * executes once and calls the startup of UltimaLive.  UltimaLive in turn rehooks this 
 * function with the permanent one.  
 *
 * @param lpFileName             The name of the file or device to be created or opened.
 * @param dwDesiredAccess        The requested access to the file or device, which can be summarized as read, write, both or neither.
 * @param dwShareMode            The requested sharing mode of the file or device, which can be read, write, both, delete, all of these, or none.
 * @param lpSecurityAttributes   A pointer to a SECURITY_ATTRIBUTES structure that contains two separate but related data members.
 * @param dwCreationDisposition  An action to take on a file or device that exists or does not exist.
 * @param dwFlagsAndAttributes   The file or device attributes and flags, FILE_ATTRIBUTE_NORMAL being the most common default value for files.
 * @param hTemplateFile          A valid handle to a template file with the GENERIC_READ access right.
 *
 * @return New File's Handle
 */
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

/**
 * @brief This function is hooked by UltimaLive's startup function. It relays the CreateFileA function to the UltimaLive 
 * file manager, which in turn relays it to any UltimaLive subscribers to process before optionally calling the original 
 * function.
 *
 * @param lpFileName             The name of the file or device to be created or opened.
 * @param dwDesiredAccess        The requested access to the file or device, which can be summarized as read, write, both or neither.
 * @param dwShareMode            The requested sharing mode of the file or device, which can be read, write, both, delete, all of these, or none.
 * @param lpSecurityAttributes   A pointer to a SECURITY_ATTRIBUTES structure that contains two separate but related data members.
 * @param dwCreationDisposition  An action to take on a file or device that exists or does not exist.
 * @param dwFlagsAndAttributes   The file or device attributes and flags, FILE_ATTRIBUTE_NORMAL being the most common default value for files.
 * @param hTemplateFile          A valid handle to a template file with the GENERIC_READ access right.
 *
 * @return New File's Handle
 */
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

/**
 * This method is hooked into the client's MapViewOfFile function. It relays control to one of UltimaLive's 
 * concrete file managers.  
 *
 * @param hFileMappingObject     A handle to a file mapping object.
 * @param dwDesiredAccess        The type of access to a file mapping object, which determines the protection of the pages.
 * @param dwFileOffsetHigh       A high-order DWORD of the file offset where the view begins.
 * @param dwFileOffsetLow        A low-order DWORD of the file offset where the view is to begin.
 * @param dwNumberOfBytesToMap   The number of bytes of a file mapping to map to the view.
 *
 * @return The starting address of the mapped view
 */
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

/**
 * @brief This method is hooked into the client's MessageBox function.
 *
 * @param hWnd       A handle to the owner window of the message box to be created.
 * @param lpText     The message to be displayed.
 * @param lpCaption  The dialog box title.
 * @param uType      The contents and behavior of the dialog box.
 *
 * @return If a message box has a Cancel button, the function returns the IDCANCEL value if either the 
 *         ESC key is pressed or the Cancel button is selected. If the message box has no Cancel button, 
 *         pressing ESC has no effect.
 */
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

/**
 * @brief This method is hooked into the client's CreateFileMapping function. It redirects control to one of 
 *        UltimaLive's concrete FileManager classes.
 * 
 * @param hFile              A handle to the file from which to create a file mapping object.
 * @param lpAttributes       A pointer to a SECURITY_ATTRIBUTES structure that determines whether a returned handle can be inherited by child processes.
 * @param flProtect          Specifies the page protection of the file mapping object.
 * @param dwMaximumSizeHigh  The high-order DWORD of the maximum size of the file mapping object.
 * @param dwMaximumSizeLow   The low-order DWORD of the maximum size of the file mapping object.
 * @param lpName             The name of the file mapping object.
 *
 * @return If the function succeeds, the return value is a handle to the newly created file mapping object.
 */
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

/**
 * @brief This method is hooked into the client's CloseHandle function.  It redirects control to one of
 *        UltimaLive's concrete FileManager classes.
 *
 * @param hObject A valid handle to an open object.
 *
 * @return If the function succeeds, the return value is nonzero.
 */
BOOL WINAPI ClientRedirections::OnCloseHandle(_In_  HANDLE hObject)
{
  BaseFileManager* pManager = UltimaLive::g_pUltimaLive->m_pClient->GetFileManager();
  if (pManager != NULL)
  {
    pManager->OnCloseHandle(hObject);
  }

  return CloseHandle(hObject);
}
