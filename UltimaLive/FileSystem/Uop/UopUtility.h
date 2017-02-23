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

#ifndef _UOP_UTILITIES_H
#define _UOP_UTILITIES_H
#include <stdint.h>
#include <string>
#include <map>
#include <list>
#include <sstream>
#include <fstream>
#include "UopStructs.h"
#include "..\..\Utils.h"
#include <algorithm>

class ProgressBarDialog;

/**
 * @class UopUtility
 *
 * @brief Utilty Class for handling aspects of the UOP file format
 */
class UopUtility
{
  public:
    static uint64_t HashFileName(std::string s);
    static std::map<uint32_t, uint64_t>* getMapHashes(int count, std::string pattern);
    static uint32_t getUopMapSizeInBytes(std::string filename);
    static void convertUopMapToMul(std::string uopSourceFilename, std::string uopDestFilename, ProgressBarDialog* pProgress);
};

#endif
