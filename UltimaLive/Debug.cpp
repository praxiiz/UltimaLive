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

#include "Debug.h"

Logger* Logger::g_pLogger = new ConsoleLogger();

#ifdef DEBUG
void ConsoleLogger::InitializeLogger()
{
  if (AttachConsole(ATTACH_PARENT_PROCESS) || AttachConsole(GetCurrentProcessId()) || AllocConsole())
  {
    SetConsoleTitleA("UltimaLive Debug Console");

    //Redirect STDOUT
    long lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
    int hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);    //Associates a C run-time file descriptor with an existing operating-system file handle.
    FILE* fp = _fdopen( hConHandle, "r+" );
    *stdout = *fp;
    setvbuf( stdout, NULL, _IONBF, 0 );
    std::ios::sync_with_stdio();
    m_hConOut = CreateFileA("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0 );

    //Redirect STDIN
    lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen( hConHandle, "r+" );
    *stdin = *fp;
    setvbuf( stdin, NULL, _IONBF, 0 );

    //Redirect STDERR
    lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen( hConHandle, "r+" );
    *stderr = *fp;
    setvbuf( stderr, NULL, _IONBF, 0 );

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

    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
    BOOL successfullyGotScreenBufferInfo = GetConsoleScreenBufferInfo( m_hConOut, &csbiInfo );

    if (successfullyGotScreenBufferInfo)
    {
      COORD coord;
      coord.X = 100;
      coord.Y = csbiInfo.dwSize.Y;
      SetConsoleScreenBufferSize( m_hConOut, coord);

      SMALL_RECT rect = csbiInfo.srWindow;
      rect.Right = rect.Left + 99;

      SetConsoleWindowInfo(m_hConOut, TRUE, &rect);
    }

      SetConsoleTextAttribute(m_hConOut, WHITE);
  }
}

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