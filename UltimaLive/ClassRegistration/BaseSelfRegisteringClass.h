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

#ifndef _BASE_SELF_REGISTERING_CLASS_HPP
#define _BASE_SELF_REGISTERING_CLASS_HPP

#include <iostream>
#include <string>
#include <vector>
#include "..\Debug.h"

/**
 * @class BaseSelfRegisteringClass
 *
 * @brief Base class that enables classes to self register (typically as factories)
 */
class BaseSelfRegisteringClass
{
protected:
  BaseSelfRegisteringClass (void (*)(), bool(*)());

public:
  virtual ~BaseSelfRegisteringClass ();

  static void Configure();
  static bool Initialize();

private:
  static std::vector <bool (*) (void)>& getInitalizationFunctions();
  static std::vector <void (*) (void)>& getConfigurationFunctions();
};
#endif
