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

#ifndef _UOP_STRUCTS_H
#define _UOP_STRUCTS_H

#include <stdint.h>
#include <iostream>

class UopHeader
{
  public:
    void unmarshal(uint8_t* pData);
    uint32_t FileIdentifier;  //myp\0
    uint32_t Version; //0x00000005
    uint32_t Signature; 
    uint64_t FileTableOffset; 
    uint32_t FileTableCapacity;
    uint32_t TotalFiles;
    uint32_t NumFileTables;
};

class FileEntry
{
  public:
    void unmarshal(uint8_t* pData);
    uint64_t UopFileOffset;
    uint32_t MetaDataSize;
    uint32_t CompressedDataSize;
    uint32_t UncompressedDataSize;
    uint64_t PathChecksum;
    uint32_t MetadataCrc;
    uint16_t CompressionMethod;
};

class FileTable
{
  public:
    void unmarshal(uint8_t* pData);
    uint32_t Capacity;
    uint64_t OffsetOfNextFileTable;
    FileEntry* pEntries;
};

class UopMetaData
{
  public:
    uint16_t unknown;
    uint16_t metaDataSize;
    uint16_t signatureId;
    uint16_t signatureLength;
    uint8_t* pSignature;
};

class UopFile
{
  public:
    UopMetaData MetaData;
    uint8_t* pData;
};
#endif
