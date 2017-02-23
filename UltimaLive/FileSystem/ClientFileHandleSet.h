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

#ifndef _CLIENT_FILE_SET_H
#define _CLIENT_FILE_SET_H

#include <string>
#include <windows.h>
#include <stdint.h>

/**
 * @class ClientFileHandleSet
 *
 * @brief Contains pointers and handles for createfile, mapviewoffile, filesize, etc
*/
class ClientFileHandleSet
{
  public:
    ClientFileHandleSet(std::string filename);
    std::string m_filename; //!< Base file name
    DWORD m_fileSizeHigh; //!< High Double word of file size
    DWORD m_fileSizeLow; //!< Low double word of file size
    HANDLE m_createFileHandle; //!< Handle returned from CreateFile
    HANDLE m_createFileMappingHandle; //!< Handle returnd from CreateFileMapping
    HANDLE m_mapFileViewHandle; //!< Handle returned from FileView
};

#endif