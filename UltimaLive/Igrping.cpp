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


#include "Igrping.h"
using namespace std;

typedef bool (__cdecl *SendPingMessageSignature)(char*, int, char*, char*, int);

/**
 * @brief Mimic the assignment operator for the CIgr_pinger class method exported by the original Igrping.DLL
 */
extern "C"
  __declspec(dllexport) CIgr_pinger& __thiscall CIgr_pinger::operator=(CIgr_pinger const &rOther)
{
  Logger::g_pLogger->LogPrint("IGRPING = OPERATOR CALLED\n");
  return (CIgr_pinger&)rOther;
}

/**
 * @brief Mimic the Constructor for the CIgr_pinger class method exported by the original Igrping.DLL
 */
extern "C"
  __declspec(dllexport) 
  CIgr_pinger::CIgr_pinger()
{
  Logger::g_pLogger->LogPrint("IGRPING CONSTRUCTOR CALLED\n");
}

/**
 * @brief Mimic the SendPingMessage method exported by the original Igrping.DLL
 * 
 * @param a unknown
 * @param b unknown
 * @param c unknown
 * @param d unknown
 * @param e unknown
 *
 * @return unknown
 */
__declspec(dllexport)
  bool __cdecl SendPingMessage(char* a, int b, char* c, char* d, int e)
{
  Logger::g_pLogger->LogPrint("IGRPING SEND PING MESSAGE\n");

  HMODULE hLib = 0L;
  SendPingMessageSignature hProc = 0L;
  bool ret = false;

  hLib = LoadLibrary("igrping_.dll");

  if (hLib != 0L)
  {
    hProc = (SendPingMessageSignature) GetProcAddress(hLib, "SendPingMessage");

    if (hProc != 0L)
    {
      ret = hProc(a, b, c, d, e);
    }
    FreeLibrary(hLib);
  }

  return ret;
}

bool CIgr_pinger::g_firstRun = true;

/**
 * @brief Entry point for UltimaLive. Hooks the CreateFileA client function for further setup.
 *
 * @verbose The processing in this method should not be done here because a deadlock can be 
 * caused by Lock Order Inversion.
 *
 * See: http://download.microsoft.com/download/a/f/7/af7777e5-7dcd-4800-8a0a-b18336565f5b/dll_bestprac.doc
 *
 * The reason that this risk is being taken is to avoid having the need for a dedicated loading program
 * that would load the exe suspended, spin it up, insert the proper hooks and resume it. The strategy to inject
 * UOLive code into the client uses redirection.  This strategy could easily be changed if a dedicated 
 * loader were used.
 *
 * DLL redirection was chosen so that other client assistance programs could continue to work despite
 * UOLive.
 *
 * The purpose of this function is to hook the kernel32.dll CreateFileA which is used to open all the client files.
 * All other initialization will be called during the first call to the hooked CreateFileA method to avoid unnecesary 
 * risk in this function.  
 */
BOOL APIENTRY DllMain (HINSTANCE, DWORD, LPVOID)
{
  if (CIgr_pinger::g_firstRun == true)
  {
    LocalPeHelper32 ultimaHelper(Utils::GetCurrentModuleName());

    ultimaHelper.Init();
    list<string> importedFunctionNames = ultimaHelper.getImportedFunctionNames("kernel32.dll");

    if (importedFunctionNames.size() > 0)
    {
      CIgr_pinger::g_firstRun = false;
      ultimaHelper.PatchImportedFunctionAddress("kernel32.dll", "CreateFileA", (DWORD)&ClientRedirections::OnCreateFileARunOnce);
    }
  }

  return TRUE;
}
