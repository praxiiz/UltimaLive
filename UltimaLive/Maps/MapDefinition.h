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

#ifndef _MAP_DEFINITION_H
#define _MAP_DEFINITION_H

#include <Windows.h>
#include <stdint.h>

/**
 * @class MapDefinition
 *
 * @brief Contains basic map definition data (number of tiles, map index, etc)
 */
#pragma pack(1)
class MapDefinition
{
  public:
    uint16_t mapWidthInTiles;      //!< Width of the map in tiles 
    uint16_t mapHeightInTiles;     //!< Height of the map in tiles
    uint16_t mapWrapWidthInTiles;  //!< Map Horizontal wrapping coordinate  
    uint16_t mapWrapHeightInTiles; //!< Map vertical wrapping coordinate
    uint8_t mapNumber;             //!< Map Index Number

    uint32_t TotalNumberOfBlocks(); //!< Total number of blocks in the map
};

#endif