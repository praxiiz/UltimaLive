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

#ifndef _MAP_FILE_SET_H
#define _MAP_FILE_SET_H

#include <string>
#include <windows.h>
#include <stdint.h>

/**
 * @class MapFileSet
 *
 * @brief Contains names and handles for the triple set of map files (land, statics, static indexes)
 */
class MapFileSet
{
  public:
    MapFileSet(uint32_t indexIn, std::string mapFilenameIn, std::string staticsFilenameIn, std::string staidxFilenameIn);

    uint32_t index; //!< Map set index

    std::string mapFilename; //!< Filename of map file
    std::string mapFilePath; //!< Path to map file
    HANDLE mapHandle; //!< Map file handle

    std::string staticsFilename; //!< Filename of statics file
    std::string staticsFilePath; //!< Path to statics file
    HANDLE staticsHandle; //!< Statics file handle

    std::string staidxFilename; //!< Filename of statics index file
    std::string staidxFilePath; //!< Path to statics index file
    HANDLE staidxHandle; //!<  Statics Index file handle
};

#endif