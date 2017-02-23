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


#ifndef _UTILS_H
#define _UTILS_H

#include <algorithm>
#include <fstream>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <sstream>
#include <string>
#include <string>
#include <stdio.h>
#include <stdint.h>
#include <Windows.h>

#include "resource.h"
#include "ProgressBarDialog.h"
#include "Debug.h"

/**
 * @class Utils
 *
 * @brief Utility class for UltimaLive
 */
class Utils
{
  public:
    static const int MAX_PATH_LENGTH = 2048; //!< Length Constant
    static std::string GetCurrentModuleName();
    static std::string GetCurrentModuleFullPath();
    static std::string GetCurrentPathWithoutFilename();
    static bool IsElevated();
    static std::string getFilenameFromPath(std::string path);
    static std::string getBaseFilenameWithoutExtension(std::string filename);

    static int getModuleMinorVersionLower();
    static int getModuleMinorVersionUpper();

    static std::wstring s2ws(const std::string& s);
};

#endif