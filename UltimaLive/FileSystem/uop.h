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

#ifndef _UOP_H
#define _UOP_H
#include <Windows.h>

/**
 * @class FileHeader
 *
 * @brief UOP File Header
 */
class FileHeader
{
  QWORD dataHeaderAddress;    //!< Memory address of data header
  DWORD fileHeaderLength;     //!< Length in bytes of file header
  DWORD compressedFileSize;   //!< Length in bytes of compressed header
  DWORD decompressedFileSize; //!< Length in bytes of uncompressed header
  QWORD fileHash;             //!< Hash of filename and path
  DWORD dataHeaderHash;       //!< Hash of the data header  
  WORD  compressionType;      //!< Type of compression used
};

/**
 * @class BlockHeader
 *
 * @brief UOP BlockHeader
 */
class BlockHeader
{
  DWORD numFilesInBlock; //!< Number of files in a UOP block 
  QWORD nextBlockAddr;   //!< Memory address of the next UOP block
};

/**
 * @class FormatHeader
 *
 * @brief UOP Format Header
 */
class FormatHeader
{
  DWORD myp;               //!< MYP
  DWORD version;           //!< Version
  DWORD signature;         //!< Signature
  QWORD firstBlockAddress; //!< Memory Address of the first UOP block
  DWORD maxFilesPerBlock;  //!< Maximum files per UOP block
  DWORD numFilesInPackage; //!< Number of files in UOP Package
};

#endif