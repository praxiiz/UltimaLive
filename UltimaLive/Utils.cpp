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


#include "Utils.h"
using namespace std;

/**
 * @brief converts standard string to wstring
 *
 * @param s string to convert
 *
 * @return converted wstring 
 */
std::wstring Utils::s2ws(const std::string& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}

/**
 * @brief Gets the name of the current module
 *
 * @return Name of current module
 */
std::string Utils::GetCurrentModuleName()
{
  char fullPath[MAX_PATH_LENGTH]; 
  int moduleNameLength = GetModuleFileName (NULL, fullPath, MAX_PATH_LENGTH);

  char* pBaseName = NULL;
  for (int i = 0; i < moduleNameLength - 1; ++i)
  {
    if (fullPath[i] == '\\' || fullPath[i] == '/')
    {
      pBaseName = &fullPath[i] + 1;
    }
  }

  return std::string(pBaseName);
}

/**
 * @brief Gets the path to the current process without the filename
 *
 * @return Path to the current process
 */
std::string Utils::GetCurrentPathWithoutFilename()
{
  char buffer[MAX_PATH];
  GetModuleFileName( NULL, buffer, MAX_PATH );
  string::size_type pos = string( buffer ).find_last_of( "\\/" );
  return string( buffer ).substr( 0, pos);
}

/**
 * @brief Gets the path to the current process including the filename
 *
 * @return Path to the current process including process name
 */
std::string Utils::GetCurrentModuleFullPath()
{
  char fullPath[MAX_PATH_LENGTH]; 
  GetModuleFileName (NULL, fullPath, MAX_PATH_LENGTH);
  return std::string(fullPath);
}

/**
 * @brief Returns the version information  for the current module
 *
 * @return File version info
 */
VS_FIXEDFILEINFO* getFileInfo()
{
  //get file info
  char fileName[_MAX_PATH];
  DWORD size = GetModuleFileName(GetModuleHandle("igrping.dll"), fileName, _MAX_PATH);

  Logger::g_pLogger->LogPrint(fileName);
  fileName[size] = NULL;
  DWORD handle = 0;
  size = GetFileVersionInfoSize(fileName, &handle);
  BYTE* versionInfo = new BYTE[size];
  if (!GetFileVersionInfo(fileName, handle, size, versionInfo))
  {
    Logger::g_pLogger->LogPrint("Failed to get version information\n");
    delete[] versionInfo;
    return NULL;
  }
  uint32_t len = 0;
  VS_FIXEDFILEINFO* pVersionFileInfo = NULL;
  VerQueryValue(versionInfo, "\\\\", (void**)&pVersionFileInfo, &len);
  delete[] versionInfo;

  return pVersionFileInfo;
}

/**
 * @brief Getter for UltimaLive major version
 *
 * @return Major Version
 */
int Utils::getModuleMinorVersionLower()
{
  int returnValue = 0;
  VS_FIXEDFILEINFO* pVersionFileInfo = getFileInfo();

  if (pVersionFileInfo != NULL)
  {
    returnValue = LOWORD(pVersionFileInfo->dwFileVersionLS);
  }

  return returnValue;
}

/**
* @brief Getter for UltimaLive minor version
*
* @return Minor Version
*/int Utils::getModuleMinorVersionUpper()
{
  int returnValue = 0;
  VS_FIXEDFILEINFO* pVersionFileInfo = getFileInfo();

  if (pVersionFileInfo != NULL)
  {
    returnValue = HIWORD(pVersionFileInfo->dwFileVersionLS);
  }

  return returnValue;
}


/**
 * @brief Check to see if the current process has admin rights
 *        verbose Taken from http://stackoverflow.com/questions/8046097/how-to-check-if-a-process-has-the-admin-rights
 *
 * @return True if process has admin rights
 */
bool Utils::IsElevated( ) 
{
    bool fRet = false;
    HANDLE hToken = NULL;
    if( OpenProcessToken( GetCurrentProcess( ), TOKEN_QUERY, &hToken )) 
    {
        TOKEN_ELEVATION Elevation;
        DWORD cbSize = sizeof( TOKEN_ELEVATION );
        if( GetTokenInformation( hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize )) 
        {
            fRet = Elevation.TokenIsElevated != 0;
        }
    }

    if( hToken ) 
    {
      CloseHandle( hToken );
    }

    return fRet;
}

/**
 * @brief Gets the base filename without the extension from a given path
 *
 * @param filename File path
 *
 * @return basefilename without the extension
 */
std::string Utils::getBaseFilenameWithoutExtension(std::string filename)
{
  std::string filenameToReturn(filename);

  int index = -1;

  for (int i = filename.length() - 1; i > 0; --i)
  {
    if (filename[i] == '.')
    {
      index = i;
      break;
    }
  }

  if (index >= 0)
  {
    filenameToReturn = filename.substr(0, index);
  }

  return filenameToReturn;
}

/**
* @brief Returns a filename with the extension from a path string
*
* @param path Path with filename
*
* @return Filename with extention
*/
std::string Utils::getFilenameFromPath(std::string path)
{
  std::string filenameToReturn;

  int startIdx = -1;


  for (int i = path.length() -1; i > 0; --i)
  {
    if (path[i] == '\\')
    {
      startIdx = i + 1;
      break;
    }
  }

  if (startIdx >= 0)
  {
    filenameToReturn = path.substr(startIdx, path.length() - 1);
  }

  return filenameToReturn;
}