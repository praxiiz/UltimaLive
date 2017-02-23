/* @file
 *
 * @brief Logger class header
 *
 * @verbatim
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

#include "Debug.h"

Logger* Logger::g_pLogger = new ConsoleLogger();

#ifdef DEBUG
/**
 * @brief Spawns a console window if one does not exist.  Redirects stdout to the console.
 */
void ConsoleLogger::InitializeLogger()
{
  if (AttachConsole(ATTACH_PARENT_PROCESS) || AttachConsole(GetCurrentProcessId()) || AllocConsole())
  {
    SetConsoleTitleA("UltimaLive Debug Console2");

	HWND hwnd = ::GetConsoleWindow();
	if (hwnd != NULL)
	{
		HMENU hMenu = ::GetSystemMenu(hwnd, FALSE);
		if (hMenu != NULL)
		{
			EnableMenuItem(hMenu, SC_CLOSE, MF_GRAYED);
			DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
		}
	}

	// http://stackoverflow.com/questions/311955/redirecting-cout-to-a-console-in-windows

	//FILE * freopen(const char * filename, const char * mode, FILE * stream)
	errno_t err;
	FILE* pStream = NULL;
	err = freopen_s(&pStream, "CONIN$", "r", stdin);
	err = freopen_s(&pStream, "CONOUT$", "w", stdout);
	err = freopen_s(&pStream, "CONOUT$", "w", stderr);
	
	std::wcout.clear();
	std::cout.clear();
	std::wcerr.clear();
	std::cerr.clear();
	std::wcin.clear();
	std::cin.clear();

	m_hConOut = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleTextAttribute(m_hConOut, WHITE);
  }
}

/**
 * @brief Logs a packet to the console with appropriate colors.
 *
 * @param fmt Printf format for the log statement
 * @param ... arguments to be passed to the printf command
 */
void ConsoleLogger::LogPacketToClient(const char* fmt, ...) 
{
  CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
  bool successfullyGotScreenBufferInfo = GetConsoleScreenBufferInfo( m_hConOut, &csbiInfo ) != 0;
  SetConsoleTextAttribute(m_hConOut, GRAY);
  printCurrentDateTime();
  SetConsoleTextAttribute(m_hConOut, TEAL);
  printf(" [PACKET] ");
  va_list ap;
  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);

  if (successfullyGotScreenBufferInfo)
  {
    SetConsoleTextAttribute(m_hConOut, csbiInfo.wAttributes);
  }
}

/**
 * @brief Logs a packet to the console with appropriate colors.
 *
 * @param fmt Printf format for the log statement
 * @param ... arguments to be passed to the printf command
 */
void ConsoleLogger::LogPacketToServer(const char* fmt, ...)
{
  CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
  bool successfullyGotScreenBufferInfo = GetConsoleScreenBufferInfo( m_hConOut, &csbiInfo ) != 0;
  SetConsoleTextAttribute(m_hConOut, GRAY);
  printCurrentDateTime();
  SetConsoleTextAttribute(m_hConOut, PINK);
  printf(" [PACKET] ");
  va_list ap;
  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);

  if (successfullyGotScreenBufferInfo)
  {
    SetConsoleTextAttribute(m_hConOut, csbiInfo.wAttributes);
  }
}

/**
 * @brief Logs a message to the console log with an [INFO] printed in front of it
 *
 * @param fmt Printf format for the log statement
 * @param ... arguments to be passed to the printf command
 */
void ConsoleLogger::LogPrint(const char* fmt, ...)
{
  CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
  bool successfullyGotScreenBufferInfo = GetConsoleScreenBufferInfo( m_hConOut, &csbiInfo ) != 0;
  SetConsoleTextAttribute(m_hConOut, GRAY);
  printCurrentDateTime();
  SetConsoleTextAttribute(m_hConOut, WHITE);
  printf(" [INFO] ");
  va_list ap;
  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);

  if (successfullyGotScreenBufferInfo)
  {
    SetConsoleTextAttribute(m_hConOut, csbiInfo.wAttributes);
  }
}

/** 
 * @brief Logs a message to the console log with an [WARNING] printed in front of it
 *
 * @param fmt Printf format for the log statement
 * @param ... arguments to be passed to the printf command
 */
void ConsoleLogger::LogPrintWarning(const char* fmt, ...)
{
  CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
  bool successfullyGotScreenBufferInfo = GetConsoleScreenBufferInfo( m_hConOut, &csbiInfo ) != 0;

  SetConsoleTextAttribute(m_hConOut, GRAY);
  printCurrentDateTime();
  SetConsoleTextAttribute(m_hConOut, YELLOW);
  printf(" [WARNING] ");

  va_list ap;
  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);

  if (successfullyGotScreenBufferInfo)
  {
    SetConsoleTextAttribute(m_hConOut, csbiInfo.wAttributes);
  }
}

/**
 * @brief Logs a message to the console log with an [ERROR] printed in front of it
 *
 * @param fmt Printf format for the log statement
 * @param ... arguments to be passed to the printf command
 */
void ConsoleLogger::LogPrintError(const char* fmt, ...)
{
  CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
  bool successfullyGotScreenBufferInfo = GetConsoleScreenBufferInfo( m_hConOut, &csbiInfo ) != 0;

  SetConsoleTextAttribute(m_hConOut, GRAY);
  printCurrentDateTime();
  SetConsoleTextAttribute(m_hConOut, RED);

  printf(" [ERROR] ");
  va_list ap;
  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);

  if (successfullyGotScreenBufferInfo)
  {
    SetConsoleTextAttribute(m_hConOut, csbiInfo.wAttributes);
  }
}

/**
 * @brief Logs the last error message reported by Windows with a [WINDOWS ERROR] printed in front of it
 */
void ConsoleLogger::LogLastErrorMessage()
{
  CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
  bool successfullyGotScreenBufferInfo = GetConsoleScreenBufferInfo( m_hConOut, &csbiInfo ) != 0;

  SetConsoleTextAttribute(m_hConOut, GRAY);
  printCurrentDateTime();
  SetConsoleTextAttribute(m_hConOut, RED);

  DWORD err = GetLastError();
  LPTSTR Error = 0;

  if(FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0, (LPTSTR)&Error, 0, NULL) == 0)
  {
    printf(" [WINDOWS ERROR] #%i\n", err);
  }
  else
  {
    printf(" [WINDOWS ERROR] %s\n", Error);
  }

  if (successfullyGotScreenBufferInfo)
  {
    SetConsoleTextAttribute(m_hConOut, csbiInfo.wAttributes);
  }
}

/** 
 * @brief Logs a message to the console log without any logger decoration
 *
 * @param fmt Printf format for the log statement
 * @param ... arguments to be passed to the printf command
 */
void ConsoleLogger::LogPrintWithoutDate(const char* fmt, ...)
{
  CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
  bool successfullyGotScreenBufferInfo = GetConsoleScreenBufferInfo( m_hConOut, &csbiInfo ) != 0;
  SetConsoleTextAttribute(m_hConOut, WHITE);

  va_list ap;
  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);

  if (successfullyGotScreenBufferInfo)
  {
    SetConsoleTextAttribute(m_hConOut, csbiInfo.wAttributes);
  }
}

/**
 * @brief Logs a warning message to the console log without date information
 *
 * @param fmt Printf format for the log statement
 * @param ... arguments to be passed to the printf command
 */
void ConsoleLogger::LogPrintWithoutDateWarning(const char* fmt, ...)
{
  CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
  bool successfullyGotScreenBufferInfo = GetConsoleScreenBufferInfo( m_hConOut, &csbiInfo ) != 0;
  SetConsoleTextAttribute(m_hConOut, YELLOW);

  va_list ap;
  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);

  if (successfullyGotScreenBufferInfo)
  {
    SetConsoleTextAttribute(m_hConOut, csbiInfo.wAttributes);
  }
}

/**
 * @brief Logs an error message to the console log without date information
 *
 * @param fmt Printf format for the log statement
 * @param ... arguments to be passed to the printf command
 */
void ConsoleLogger::LogPrintWithoutDateError(const char* fmt, ...)
{
  CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
  bool successfullyGotScreenBufferInfo = GetConsoleScreenBufferInfo( m_hConOut, &csbiInfo ) != 0;
  SetConsoleTextAttribute(m_hConOut, RED);

  va_list ap;
  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);

  if (successfullyGotScreenBufferInfo)
  {
    SetConsoleTextAttribute(m_hConOut, csbiInfo.wAttributes);
  }
}

/**
 * @brief Logs a message with a success or failure status message
 *
 * @param status True for success, false for failure
 */
void ConsoleLogger::LogPrintTaskStatusResult(bool status)
{
  CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
  bool successfullyGotScreenBufferInfo = GetConsoleScreenBufferInfo( m_hConOut, &csbiInfo ) != 0;


  printf("[");

  if (status)
  {
    SetConsoleTextAttribute(m_hConOut, GREEN);
    printf("SUCCESS");
  }
  else
  {
    SetConsoleTextAttribute(m_hConOut, RED);
    printf("FAILED");
  }

  if (successfullyGotScreenBufferInfo)
  {
    SetConsoleTextAttribute(m_hConOut, csbiInfo.wAttributes);
  }

  printf("]\n");
}
#endif

/**
 * @brief Logs the current date and time to the console
 */
void ConsoleLogger::printCurrentDateTime() 
{
  time_t rawtime;
  tm timeinfo;
  char buffer [80];

  time (&rawtime);
  localtime_s (&timeinfo, &rawtime);
  strftime (buffer,80,"%Y-%m-%d %H:%M:%S ", const_cast<const tm *>(&timeinfo));
  printf("%s", buffer);
}