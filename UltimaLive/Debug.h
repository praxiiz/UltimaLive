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

#ifndef _DEBUG_H
#define _DEBUG_H

#include <ctime>
#include <fstream>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <iomanip>
#include <ostream>
#include <stdarg.h>
#include <stdio.h>
#include <string>
#include <Windows.h>

class Logger
{
  public:
    virtual void LogPrint(const char *, ...) = 0;
    virtual void LogPrintWarning(const char *, ...) = 0;
    virtual void LogPrintError(const char *, ...) = 0;

    virtual void LogLastErrorMessage() = 0;

    virtual void LogPacketToServer(const char *, ...) = 0;
    virtual void LogPacketToClient(const char *, ...) = 0;

    virtual void LogPrintWithoutDate(const char *, ...) = 0;
    virtual void LogPrintWithoutDateWarning(const char *, ...) = 0;
    virtual void LogPrintWithoutDateError(const char *, ...) = 0;

    virtual void LogPrintTaskStatusResult(bool status) = 0;

    virtual void InitializeLogger() = 0;
  
    static Logger* g_pLogger;
}; 

class ConsoleLogger : public Logger
{
  public:
    enum Color { DARKBLUE = 1, DARKGREEN, DARKTEAL, DARKRED, DARKPINK, DARKYELLOW, GRAY, DARKGRAY, BLUE, GREEN, TEAL, RED, PINK, YELLOW, WHITE };

  #ifdef DEBUG
    void LogPrint(const char *, ...);
    void LogPrintWarning(const char *, ...);
    void LogPrintError(const char *, ...);
    void LogPacketToServer(const char *, ...); 
    void LogPacketToClient(const char *, ...); 

    void LogLastErrorMessage();

	  void LogPrintWithoutDate(const char *, ...);
	  void LogPrintWithoutDateWarning(const char *, ...);
	  void LogPrintWithoutDateError(const char *, ...);

    void LogPrintTaskStatusResult(bool status);
    void InitializeLogger();
  #else
    inline void LogPrint(const char *, ...) {};
    inline void LogPrintWarning(const char *, ...) {};
    inline void LogPrintError(const char *, ...) {};
    inline void LogPacketToServer(const char *, ...) {};
    inline void LogPacketToClient(const char *, ...) {};

    inline void LogLastErrorMessage() {};

	  inline void LogPrintWithoutDate(const char *, ...) {};
	  inline void LogPrintWithoutDateWarning(const char *, ...) {};
	  inline void LogPrintWithoutDateError(const char *, ...) {};

    inline void LogPrintTaskStatusResult(bool status) {};
    inline void InitializeLogger() {};
  #endif 

  private:
    HANDLE m_hConOut;
    void printCurrentDateTime();
};

#endif