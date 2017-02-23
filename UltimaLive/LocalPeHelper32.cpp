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

#include "LocalPeHelper32.hpp"
using namespace std;

/**
 * @brief Constructor for LocalPeHelper32
 * 
 * @param moduleName name of the module, e.g. "kernel32.dll:
 */
LocalPeHelper32::LocalPeHelper32(std::string moduleName)
  : m_initialized(false),
  m_pBaseAddress(NULL),
  m_moduleName(moduleName),
  m_pDosHeader(NULL),
  m_pNtHeader(NULL),
  m_pExportDirectory(NULL),
  m_pImportDescriptor(NULL)
{
  //do nothing
}

/**
 * @brief converts a string to uppercase
 *
 * @param s String to convert
 *
 * @return Uppercase string
 */
std::string toUpper(const std::string & s)
{
    std::string ret(s.size(), char());
    for(unsigned int i = 0; i < s.size(); ++i)
        ret[i] = (s[i] <= 'z' && s[i] >= 'a') ? s[i]-('a'-'A') : s[i];
    return ret;
}

/**
 * @brief Searches for the module and upon finding it, sets pointers to the regular PE structures.
 *        Sets a flag true if the module has been found and the pointers set.
 */
bool LocalPeHelper32::Init()
{
  m_initialized = false;

  std::list<MODULEENTRY32> localModules = GetLocalModuleList();

  for(std::list<MODULEENTRY32>::iterator i = localModules.begin(); i != localModules.end(); ++i)
  {
    if (toUpper(i->szModule) == toUpper(m_moduleName))
    {
      m_pBaseAddress = i->modBaseAddr;
      m_initialized = true;
    }
  }

  if (m_initialized == true)
  {
    m_pDosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(m_pBaseAddress);
    m_pNtHeader = reinterpret_cast<IMAGE_NT_HEADERS*>(m_pBaseAddress + m_pDosHeader->e_lfanew);
    m_pExportDirectory = reinterpret_cast<IMAGE_EXPORT_DIRECTORY*>(m_pBaseAddress + 
	    m_pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
    m_pImportDescriptor = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(m_pBaseAddress + 
      m_pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
  }

  return m_initialized;
}

/**
 * @brief Prints out a memory location in hex editor style.
 * 
 * @param buffer Pointer to the memory to print (unsigned char)
 * @param size Length of memory to print
 */
void LocalPeHelper32::HexPrint(unsigned char* buffer, int size)
{
  char asciiLine[17] = { };
  asciiLine[16] = NULL;

  for ( int i = 0; i < 16; i++)
  {
    asciiLine[i] = '.';
  }

  int i = 0;
  for (i = 0; i < size; i++)
  {
    if (i == 0 || ((i % 16) == 0)) 
    {
      Logger::g_pLogger->LogPrint(" %0.8x  |", (buffer + i));
    }
    else if ((i % 8) == 0) 
    {
      Logger::g_pLogger->LogPrint("  ");
    }
    
    Logger::g_pLogger->LogPrint(" %0.2x", buffer[i]);

    if (buffer[i] > 32 && buffer[i] < 127)
    {
      asciiLine[i % 16] = (char)buffer[i];
    }
    else
    {
      asciiLine[i % 16] = '.';
    }

    if (i > 0 && ((i + 1) % 16) == 0)
    {
      Logger::g_pLogger->LogPrint(" | %s |\n", asciiLine);
      for ( int j = 0; j < 16; j++)
      {
        asciiLine[j] = '.';
      }
    }
  }

  if (i % 16 != 0)
  {
    for (int j = i % 16; j < 16; j++)
    {
      Logger::g_pLogger->LogPrint("   ");
      asciiLine[j] = ' ';
      if ((j % 8) == 0) 
      {
        Logger::g_pLogger->LogPrint("  ");
      }
    }
    Logger::g_pLogger->LogPrint(" | %s |\n", asciiLine);

  }

  Logger::g_pLogger->LogPrint("\n");
}

/**
 * @brief Prints out a memory location in hex editor style.
 *
 * @param buffer Pointer to the memory to print (char*)
 * @param size Length of memory to print
 */
void LocalPeHelper32::HexPrint(char* buffer, int size)
{
  char asciiLine[16] = { };
  for ( int i = 0; i < 16; i++)
  {
    asciiLine[i] = '.';
  }

  for (int i = 0; i < size; i++)
  {
    if (i == 0 || ((i % 16) == 0)) 
    {
      Logger::g_pLogger->LogPrint(" %0.8x  |", (buffer + i));
    }
    else if ((i % 8) == 0) 
    {
      Logger::g_pLogger->LogPrint("  ");
    }
    
    Logger::g_pLogger->LogPrint(" %0.2x", (unsigned char)buffer[i]);

    if ((unsigned char)buffer[i] > 32 && (unsigned char)buffer[i] < 127)
    {
      asciiLine[i % 16] = buffer[i];
    }
    else
    {
      asciiLine[i % 16] = '.';
    }

    if (i > 0 && ((i + 1) % 16) == 0)
    {
      Logger::g_pLogger->LogPrint(" | %s |\n", asciiLine);
      for ( int j = 0; j < 16; j++)
      {
        asciiLine[j] = '.';
      }
    }
  }
  Logger::g_pLogger->LogPrint("\n");
}

/**
 * @brief Gets a list of modules loaded and linked to this module.
 *
 * @return List of Module Entries
 */
std::list<MODULEENTRY32> LocalPeHelper32::GetLocalModuleList()
{
  std::list<MODULEENTRY32> resultList;
    
  //get a snapshot of the process
  HANDLE hModuleSnapshot;
  MODULEENTRY32 moduleEntry;
  hModuleSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
  if (hModuleSnapshot != INVALID_HANDLE_VALUE)
  {
    moduleEntry.dwSize = sizeof(MODULEENTRY32);
    if(Module32First( hModuleSnapshot, &moduleEntry ) ) 
    { 
      do 
      { 
        resultList.push_back(moduleEntry);
      } while( Module32Next( hModuleSnapshot, &moduleEntry ) );
    }
  }
  return resultList;
}

/**
 * @brief searches the Import Address Table (IAT) for the memory address of a function that has been imported in a given DLL
 *
 * @param dllName Name of the Dynamic Link Library function
 * @param functionName Name of the function
 * 
 * @return Address of the function as a DWORD
 */
DWORD LocalPeHelper32::getImportedFunctionAddress(std::string dllName, std::string functionName)
{
  DWORD result = NULL;
  IMAGE_IMPORT_DESCRIPTOR* pDescriptorItr = matchImportDescriptor(dllName);

  if (pDescriptorItr != NULL)
  {
    DWORD* pImportItrAddr = reinterpret_cast<DWORD*>(pDescriptorItr->FirstThunk + (DWORD)m_pBaseAddress);
    for (DWORD* pLookupItrAddr = reinterpret_cast<DWORD*>(pDescriptorItr->OriginalFirstThunk + (DWORD)m_pBaseAddress); 
      *pLookupItrAddr != NULL; pLookupItrAddr++)
    {
        if ((*pLookupItrAddr & 0x80000000) == 0x00000000) //lookup by ordinal
        {
          DWORD hintAddress = (DWORD)(*pLookupItrAddr & ~0x80000000) + (DWORD)m_pBaseAddress;
          string currentFunctionName = std::string(reinterpret_cast<char*>(hintAddress + 2));

          if (toUpper(currentFunctionName) == toUpper(functionName))
          {
            result = *pImportItrAddr;
            break;
          }          
        }
        pImportItrAddr++;
    }
  }

  return result;
}

/**
 * @brief searches the export table for the memory address of a function that has been exported by a given DLL
 *
 * @param dllName Name of the Dynamic Link Library function
 * @param functionName Name of the function
 *
 * @return Address of the function as a DWORD
 */
DWORD LocalPeHelper32::getExportedFunctionAddress(std::string dllName, std::string functionName)
{
  DWORD addressValue = NULL;

  WORD* nameOrdinals = reinterpret_cast<WORD*>(m_pExportDirectory->AddressOfNameOrdinals + (DWORD)m_pBaseAddress);
  char** nameAddresses = reinterpret_cast<char**>(m_pExportDirectory->AddressOfNames + (DWORD)m_pBaseAddress);

  if (nameOrdinals != NULL)
  {
    for (unsigned int i = 0; i < m_pExportDirectory->NumberOfNames; i++)
    {
      WORD ordinal = nameOrdinals[i];
      char* pName = reinterpret_cast<char*>(nameAddresses[ordinal] + (DWORD)m_pBaseAddress);
      string functionNameItr(pName);
      if (toUpper(pName) == toUpper(functionNameItr))
      {
        DWORD* addresses = reinterpret_cast<DWORD*>(m_pExportDirectory->AddressOfFunctions + (DWORD)m_pBaseAddress);
        addressValue = addresses[ordinal];
      }
    }
  }

  return addressValue;
}

/**
 * @brief Patches an imported address in a DLL
 *
 * @param dllName Name of the Dynamic Link Library function
 * @param functionName Name of the function
 * @param newAddress New function address to patch into the import address table
 *
 * @return Address of the function as a DWORD
 */
bool LocalPeHelper32::PatchImportedFunctionAddress(std::string dllName, std::string functionName, DWORD newAddress)
{
  bool result = false;
  IMAGE_IMPORT_DESCRIPTOR* pDescriptorItr = matchImportDescriptor(dllName);

  if (pDescriptorItr != NULL)
  {
    DWORD* pImportItrAddr = reinterpret_cast<DWORD*>(pDescriptorItr->FirstThunk + (DWORD)m_pBaseAddress);
    for (DWORD* pLookupItrAddr = reinterpret_cast<DWORD*>(pDescriptorItr->OriginalFirstThunk + (DWORD)m_pBaseAddress); 
      *pLookupItrAddr != NULL; pLookupItrAddr++)
    {
        if ((*pLookupItrAddr & 0x80000000) == 0x00000000) //lookup by ordinal
        {
          DWORD hintAddress = (DWORD)(*pLookupItrAddr & ~0x80000000) + (DWORD)m_pBaseAddress;
          string currentFunctionName = std::string(reinterpret_cast<char*>(hintAddress + 2));

          if (toUpper(currentFunctionName) == toUpper(functionName))
          {
            if (SetMemoryWritable((DWORD)pImportItrAddr) == 0)
            {
              *pImportItrAddr = newAddress;
              result = true;
              break;
            }
          }          
        }
        pImportItrAddr++;
    }
  }

  return result;
}

/**
 * @brief Sets a section of memory writable in the current process
 *
 * @param startingAddress starting address of the memory section 
 *
 * @return return value from the VirtualQueryEx function call
 */
DWORD LocalPeHelper32::SetMemoryWritable(DWORD startingAddress)
{
  HANDLE hProcess = GetCurrentProcess();

  DWORD errorCode = 0;

  //get the page baseaddress of the page where our address resides
  MEMORY_BASIC_INFORMATION mem_info;
  DWORD old_protections = 0;

  // VirtualQueryEx
  // The return value is the actual number of bytes returned in the information buffer.
  // If the function fails, the return value is zero. 
	DWORD retval = VirtualQueryEx(hProcess, (LPCVOID)startingAddress, &mem_info, sizeof(MEMORY_BASIC_INFORMATION));

	if (retval == 0)
	{
    errorCode = GetLastError();
    Logger::g_pLogger->LogPrint("VirtualQueryEx failed, error code: %i\n", errorCode);
    Logger::g_pLogger->LogPrint("VirtualQueryEx failed, retvalue: %i\n", retval);
    
	}
  else if(VirtualProtectEx(hProcess, mem_info.BaseAddress, mem_info.RegionSize, PAGE_READWRITE, &old_protections) == 0)
	{
    errorCode = GetLastError();
    Logger::g_pLogger->LogPrint("VirtualProtectEx failed: %i\n", errorCode);
    Logger::g_pLogger->LogPrint("param 1: %x\n", hProcess);
    Logger::g_pLogger->LogPrint("param 2: %x\n", mem_info.BaseAddress);
    Logger::g_pLogger->LogPrint("param 3: %x\n", mem_info.RegionSize);
    Logger::g_pLogger->LogPrint("param 4: %x\n", PAGE_READWRITE);
    Logger::g_pLogger->LogPrint("param 5: %x\n", old_protections);

	}

	return errorCode;
}

/**
 * @brief Find the import descriptor in the current process for a given DLL
 *
 * @param dllName DLL Name to search
 *
 * @return Import Descriptor of the DLL or NULL it it was not found
 */
IMAGE_IMPORT_DESCRIPTOR* LocalPeHelper32::matchImportDescriptor(std::string dllName)
{
  IMAGE_IMPORT_DESCRIPTOR* pResult = NULL;

  for (IMAGE_IMPORT_DESCRIPTOR* pDescriptorItr = m_pImportDescriptor; pDescriptorItr != NULL; pDescriptorItr++)
  {
    string s(reinterpret_cast<char*>(pDescriptorItr->Name + m_pBaseAddress));
    if (toUpper(s) == toUpper(dllName))
    {
      pResult = pDescriptorItr;
      break;
    }
  }

  return pResult;
}

/**
 * @brief Gets a list of all exported functions in the current module
 * 
 * @return list of strings with exported function names
 */
list<string> LocalPeHelper32::getExportedFunctionNames()
{
  std::list<std::string> functionNames;

  WORD* nameOrdinals = reinterpret_cast<WORD*>(m_pExportDirectory->AddressOfNameOrdinals + (DWORD)m_pBaseAddress);
  char** nameAddresses = reinterpret_cast<char**>(m_pExportDirectory->AddressOfNames + (DWORD)m_pBaseAddress);

  if (nameOrdinals != NULL)
  {
    for (uint32_t i = 0; i < m_pExportDirectory->NumberOfNames; i++)
    {
      WORD ordinal = nameOrdinals[i];
      char* pName = reinterpret_cast<char*>(nameAddresses[ordinal] + (DWORD)m_pBaseAddress);
      string functionName(pName);
      functionNames.push_back(functionName);
    }
  }

  return functionNames;
}

/**
 * @brief Gets a list of all imported function names in a given DLL
 *
 * @param dllName Name of the DLL to search
 *
 * @return a list of strings with imported function names
 */
list<string> LocalPeHelper32::getImportedFunctionNames(std::string dllName)
{
  std::list<std::string> functionNames;
  IMAGE_IMPORT_DESCRIPTOR* pDescriptorItr = matchImportDescriptor(dllName);

  if (pDescriptorItr != NULL)
  {  
    for (DWORD* pLookupItrAddr = reinterpret_cast<DWORD*>(pDescriptorItr->OriginalFirstThunk + (DWORD)m_pBaseAddress); 
      *pLookupItrAddr != NULL; pLookupItrAddr++)
    {
        if ((*pLookupItrAddr & 0x80000000) != 0x00000000) //lookup by ordinal
        {
          char buffer [64];
          string functionName = string("ORDINAL_");
		  _itoa_s(*pLookupItrAddr & 0x0000FFFF, buffer, 64, 10);
          functionName.append(buffer);
        }
        else
        {
          DWORD hintAddress = (DWORD)(*pLookupItrAddr & ~0x80000000) + (DWORD)m_pBaseAddress;
          string functionName = std::string(reinterpret_cast<char*>(hintAddress + 2));
          functionNames.push_back(functionName);
        }
    }
  }

  return functionNames;
}
