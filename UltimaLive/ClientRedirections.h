/**
 * @file
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

#ifndef _CLIENT_REDIRECTIONS_H
#define _CLIENT_REDIRECTIONS_H

#include <Windows.h>
#include "UltimaLive.h"
#include "Client.h"
#include "FileSystem\BaseFileManager.h"
#include "LocalPeHelper32.hpp"

/**
 * @class ClientRedirections
 *
 * @brief ClientsRedirections contains redirected client functions.
 */
class ClientRedirections
{
  public:
    static HANDLE WINAPI OnCreateFileARunOnce(__in LPCSTR lpFileName, __in DWORD dwDesiredAccess, __in DWORD dwShareMode,
      __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes, __in DWORD dwCreationDisposition, __in DWORD dwFlagsAndAttributes, __in_opt HANDLE hTemplateFile);
    static bool InstallClientHooks();
    static void OnUpdateStaticBlocks();

  private:
    static HANDLE WINAPI OnCreateFileA(__in LPCSTR lpFileName, __in DWORD dwDesiredAccess, __in DWORD dwShareMode,
      __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes, __in DWORD dwCreationDisposition, __in DWORD dwFlagsAndAttributes, __in_opt HANDLE hTemplateFile);

    static LPVOID WINAPI OnMapViewOfFile(__in HANDLE hFileMappingObject, __in DWORD dwDesiredAccess, __in DWORD dwFileOffsetHigh, __in DWORD dwFileOffsetLow,
      __in SIZE_T dwNumberOfBytesToMap);

    static HANDLE WINAPI OnCreateFileMappingA(__in HANDLE hFile, __in_opt LPSECURITY_ATTRIBUTES lpAttributes, __in DWORD flProtect,
      __in DWORD dwMaximumSizeHigh, __in DWORD dwMaximumSizeLow, __in_opt LPCSTR lpName);

    static int WINAPI OnMessageBox(_In_opt_  HWND hWnd, _In_opt_  LPCTSTR lpText, _In_opt_  LPCTSTR lpCaption, _In_ UINT uType);

    static BOOL WINAPI OnCloseHandle(_In_  HANDLE hObject);

    static void __fastcall OnReceivePacket(void* This, void* edx, unsigned char *pBuffer);

    static int __fastcall OnSendPacket(void* This, void* edx, unsigned char* pBuffer);

    static HANDLE g_UpdateStaticBlocksMutex;  //!< Mutex for the redirected UpdateStaticBlocksMutex function
};

#endif