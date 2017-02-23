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

#include "UopStructs.h"

/**
 * @brief Unmarshals a UOP header
 *
 * @param pData pointer to UOP header data
 */
void UopHeader::unmarshal(uint8_t* pData)
{
  FileIdentifier = *reinterpret_cast<uint32_t*>(pData);  //myp\0
  Version = *reinterpret_cast<uint32_t*>(pData + 4); //0x00000005
  Signature = *reinterpret_cast<uint32_t*>(pData + 8); 
  FileTableOffset = *reinterpret_cast<uint64_t*>(pData + 12);
  FileTableCapacity = *reinterpret_cast<uint32_t*>(pData + 20);
  TotalFiles = *reinterpret_cast<uint32_t*>(pData + 24);
  NumFileTables = *reinterpret_cast<uint32_t*>(pData + 28);
}

/**
 * @brief Unmarshals a UOP file entry
 *
 * @param pData pointer to UOP file entry data
 */
void FileEntry::unmarshal(uint8_t* pData)
{
  UopFileOffset = *reinterpret_cast<uint64_t*>(pData);
  MetaDataSize = *reinterpret_cast<uint32_t*>(pData + 8);
  CompressedDataSize = *reinterpret_cast<uint32_t*>(pData + 12);
  UncompressedDataSize = *reinterpret_cast<uint32_t*>(pData + 16);
  PathChecksum = *reinterpret_cast<uint64_t*>(pData + 20);
  MetadataCrc = *reinterpret_cast<uint32_t*>(pData + 28);
  CompressionMethod = *reinterpret_cast<uint16_t*>(pData + 32);
}

/**
 * @brief Unmarshals a UOP file table
 *
 * @param pData pointer to UOP file table data
 */
void FileTable::unmarshal(uint8_t* pData)
{
    //uint32_t Capacity  = *reinterpret_cast<uint32_t*>(pData);
    //uint64_t OffsetOfNextFileTable  = *reinterpret_cast<uint64_t*>(pData + 4);
    int count = 0;

    //34
    for (uint8_t* i = pData + 12; *reinterpret_cast<uint64_t*>(i) != 0; i += 34)
    {
      count++;
    }

    pEntries = new FileEntry[count];
    for (int i = 0; i < count; ++i)
    {
      pEntries[i] = FileEntry();
      pEntries[i].unmarshal(pData + i * 34 + 12);
    }
}
