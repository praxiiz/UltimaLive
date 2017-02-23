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

#ifndef _UOP_STRUCTS_H
#define _UOP_STRUCTS_H

#include <stdint.h>
#include <iostream>

/**
 * @class UopHeader
 * 
 * @brief UOP File header
 */
class UopHeader
{
  public:
	/**
	 * @brief Unmarshal a UOP Header
	 *
	 * @param pData Pointer to FileEntry data
	 */
    void unmarshal(uint8_t* pData);
    uint32_t FileIdentifier;    //!< myp\0
    uint32_t Version;           //!< 0x00000005
    uint32_t Signature; 		//!< UOP Header signature
    uint64_t FileTableOffset; 	//!< File Table offset
    uint32_t FileTableCapacity;	//!< File Table capacity
    uint32_t TotalFiles;		//!< Total number of files
    uint32_t NumFileTables;		//!< Number of file tables
};

/**
 * @class FileEntry
 *
 * @brief UOP File Entry
 */
class FileEntry
{
  public:
   /**
	* @brief Unmarshal a FileEntry
	*
	* @param pData Pointer to FileEntry data
	*/
    void unmarshal(uint8_t* pData);
    uint64_t UopFileOffset;        //!< Offset to the first file entry
    uint32_t MetaDataSize;		   //!< Size in bytes of File Entry meta data
    uint32_t CompressedDataSize;   //!< Compressed data size in bytes 
    uint32_t UncompressedDataSize; //!< Uncompressed data size in bytes
    uint64_t PathChecksum;		   //!< Cheksum of file path
    uint32_t MetadataCrc;		   //!< CRC of meta data
    uint16_t CompressionMethod;	   //!< Compression method
};

/**
 * @class FileTable
 *
 * @brief UOP FileTable
 */
class FileTable
{
  public:
	/**
	 * @brief Unmarshal a file table
	 *
	 * @param pData Pointer to file table data
	 */
    void unmarshal(uint8_t* pData);
    uint32_t Capacity;              //!< UOP File table capacity
    uint64_t OffsetOfNextFileTable;	//!< Offset to the next UOP file table
    FileEntry* pEntries;			//!< Pointer to first file entry
};

/**
 * @class UopMetaData
 *
 * @brief UOP Meta Data
 */
class UopMetaData
{
  public:
    uint16_t unknown;         //!< Unknown
    uint16_t metaDataSize;	  //!< Size of UOP meta data
    uint16_t signatureId;	  //!< Signature ID
    uint16_t signatureLength; //!< Number of bytes in Signature 
    uint8_t* pSignature;	  //!< Pointer to signature data
};

/**
 * @class UopFile
 *
 * @brief UOP File 
 */
class UopFile
{
  public:
    UopMetaData MetaData; //!< UOP meta data
    uint8_t* pData; //!< Pointer to UOP file data
};
#endif
