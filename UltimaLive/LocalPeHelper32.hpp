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


#ifndef _PE_HELPER_H
#define _PE_HELPER_H

#include <Windows.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <stdint.h>

#include <list>
#include <Winternl.h>
#include <Psapi.h>
#include <TlHelp32.h>
#include "Debug.h"

#pragma comment(lib, "Psapi.lib")
#define DPSAPI_VERSION (1)

/*
*  +-------------------------+
*  | Import Descriptor       |
*  +-------------------------+
*  | Characteristics (RVA)   | ---> Hint Name Table
*  | TimeDateStamp           |
*  | Forwarder Chain         | ---> 0xFFFFFFFF
*  | Imported DLL Name (RVA) | ---> Kernel32.dll
*  | First Thunk (RVA)       | ---> First Thunk Table (Import Address Table)
*  +-------------------------+
*
*
* There are two parallel arrays of pointers to IMAGE_IMPORT_BY_NAME.  The first array
* (pointed to by Characteristics) is left alone, and never modified. It is sometimes
* called the hint-name table.
*
* The second array (pointed to by FirstThunk) is overwritten by the PE loader. The
* loader iterates through each pointer in the array and finds the address of the
* function that each IMAGE_IMPORT_BY_NAME structure refers to. The loader then
* overwrites the pointer with the found function's address.
*
*  +----------------------+    +----------------------+
*  | Hint Name Table      |    | Import Address Table |
*  +----------------------+    +----------------------+
*  | Hint    | Name       |    | 0x... RVA            |
*  +----------------------+    +----------------------+
*  | Hint    | Name       |    | 0x... RVA            |
*  +----------------------+    +----------------------+
*  | Hint    | Name       |    | 0x... RVA            |
*  +----------------------+    +----------------------+
*  |         NULL         |    |         NULL         |
*  +----------------------+    +----------------------+
*
*  Ord Name
*   10 BeginPaint
*   35 CharUpperA
*/


/**
 * @class LocalPeHelper32
 *
 * @brief Utility class for manipulating and patching the local (current process) modules's Import and Export table
 */
class LocalPeHelper32
{
  public:
 	static std::list<MODULEENTRY32> GetLocalModuleList();
    static void HexPrint(char* buffer, int size);
    static void HexPrint(unsigned char* buffer, int size);
    static DWORD SetMemoryWritable(DWORD startingAddress);

    LocalPeHelper32(std::string moduleName);
    bool Init();

    bool PatchImportedFunctionAddress(std::string dllName, std::string functionName, DWORD newAddress);
    DWORD getImportedFunctionAddress(std::string dllName, std::string functionName);
    DWORD getExportedFunctionAddress(std::string dllName, std::string functionName);

    std::list<std::string> getImportedFunctionNames(std::string dllName);
    std::list<std::string> getExportedFunctionNames();

private:
    IMAGE_IMPORT_DESCRIPTOR* matchImportDescriptor(std::string dllName);
    IMAGE_EXPORT_DIRECTORY* matchExportDirectory(std::string dllName);
    
    bool m_initialized;       //!< Indicates that the class instance has found function pointers and has completed internal initialization
    BYTE* m_pBaseAddress;     //!< Base address of the current process
    std::string m_moduleName; //!< Module name of the current process
    IMAGE_DOS_HEADER* m_pDosHeader; //!< Pointer to the MSDOS Legacy Header
    IMAGE_NT_HEADERS* m_pNtHeader;  //!< Pointer to the NT Legacy Header
    IMAGE_EXPORT_DIRECTORY* m_pExportDirectory;   //!< Pointer to the current module's export address table (EAT)
    IMAGE_IMPORT_DESCRIPTOR* m_pImportDescriptor; //!< Pointer to the current module's import address table (IAT)
};

#endif //_PE_HELPER_H
