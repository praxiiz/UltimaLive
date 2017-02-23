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

#include "BaseSelfRegisteringClass.h"
#include <Windows.h>
/**
 * @brief Constructor for BaseSelfRegistering class
 */
BaseSelfRegisteringClass::~BaseSelfRegisteringClass ()
{
  //do nothing
}

/** 
 * @brief Overloaded constructor for BaseSelfRegistering class. Stores function pointers for later retrieval.
 * Function pointers are passed in by the SelfRegisteringClass automatically through templating
 *
 * @param config Function pointer to each class's static configure function
 * @param init Function pointer to each class's static initialization function
 */
BaseSelfRegisteringClass::BaseSelfRegisteringClass (void (*config)() , bool(*init)() )
{
	getConfigurationFunctions().push_back(config);
	getInitalizationFunctions().push_back(init);
}

/** 
 * @brief Calls all static stored class configuration functions
 */
void BaseSelfRegisteringClass::Configure()
{
  Logger::g_pLogger->LogPrint("Configuring %i classes\n", getConfigurationFunctions().size());

  for (std::vector<void (*) (void)>::iterator it = getConfigurationFunctions().begin() ; it != getConfigurationFunctions().end(); ++it)
  {
    (*it)();
  }
}

/**
 * @brief Calls all static stored class initialization functions
 */
bool BaseSelfRegisteringClass::Initialize()
{
  bool success = true;
  for (std::vector<bool (*) (void)>::iterator it = getInitalizationFunctions().begin() ; it != getInitalizationFunctions().end(); ++it)
  {
    bool result = (*it)();
    if (!result)
    {
      success = false;
    }
  }

  return success;
}

/** Uses the Construct On First Use Idiom to get the list of initialization functions. 
 *
 * These static variables must be wrapped in functions
 * See: http://stackoverflow.com/questions/1005685/c-static-initialization-order
 * and
 * see:http://www.parashift.com/c++-faq-lite/ctors.html#faq-10.12
 *
 * There is a danger to this method if not used propertly:
 * see: http://www.parashift.com/c++-faq/construct-on-first-use-v2.html
 *
 * return vector of function pointers to initialization functions
 */
std::vector <bool (*) (void)>& BaseSelfRegisteringClass::getInitalizationFunctions()
{
  static std::vector <bool (*) (void)> initializationFunctions;
  return initializationFunctions;
}

/**
 * @brief Uses the Construct On First Use Idiom to get the list of configuration functions.
 * 
 * @return vector of function pointers to configuration functions
 */
std::vector <void (*) (void)>& BaseSelfRegisteringClass::getConfigurationFunctions()
{
  static std::vector <void (*) (void)> configurationFunctions;
  return configurationFunctions;
}
