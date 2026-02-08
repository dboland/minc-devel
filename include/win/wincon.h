/**
 * @file wincon.h
 * Copyright 2012, 2013 MinGW.org project
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

typedef struct _COORD {
        SHORT X;
        SHORT Y;
} COORD, *PCOORD;

typedef struct _SMALL_RECT {
	SHORT Left; 
	SHORT Top; 
	SHORT Right; 
	SHORT Bottom; 
} SMALL_RECT; 

typedef struct _CONSOLE_SCREEN_BUFFER_INFO {
	COORD dwSize;
	COORD dwCursorPosition;
	WORD wAttributes;
	SMALL_RECT srWindow;
	COORD dwMaximumWindowSize;
} CONSOLE_SCREEN_BUFFER_INFO;

#define WriteConsole __AW(WriteConsole)
#define ReadConsole __AW(ReadConsole)

#define ENABLE_VIRTUAL_TERMINAL_PROCESSING      0x0004
#define DISABLE_NEWLINE_AUTO_RETURN		0x0008
#define ENABLE_VIRTUAL_TERMINAL_INPUT           0x0200

#define FOREGROUND_BLUE 1
#define FOREGROUND_GREEN        2
#define FOREGROUND_RED  4
#define FOREGROUND_INTENSITY    8
#define BACKGROUND_BLUE 16
#define BACKGROUND_GREEN        32
#define BACKGROUND_RED  64
#define BACKGROUND_INTENSITY    128

#define ENABLE_LINE_INPUT 2
#define ENABLE_ECHO_INPUT 4
#define ENABLE_PROCESSED_INPUT 1
#define ENABLE_WINDOW_INPUT 8
#define ENABLE_MOUSE_INPUT 16
#define ENABLE_INSERT_MODE 32
#define ENABLE_QUICK_EDIT_MODE 64
#define ENABLE_EXTENDED_FLAGS  128
#define ENABLE_AUTO_POSITION   256
#define ENABLE_PROCESSED_OUTPUT 1
#define ENABLE_WRAP_AT_EOL_OUTPUT 2

BOOL WINAPI GetConsoleMode(HANDLE,PDWORD);
BOOL WINAPI SetConsoleMode(HANDLE,DWORD);
BOOL WINAPI ReadConsoleA(HANDLE,PVOID,DWORD,PDWORD,PVOID);
BOOL WINAPI ReadConsoleW(HANDLE,PVOID,DWORD,PDWORD,PVOID);
BOOL WINAPI WriteConsole(HANDLE,PCVOID,DWORD,PDWORD,PVOID);
BOOL WINAPI WriteConsoleA(HANDLE,PCVOID,DWORD,PDWORD,PVOID);
BOOL WINAPI WriteConsoleW(HANDLE,PCVOID,DWORD,PDWORD,PVOID);
BOOL WINAPI SetConsoleTextAttribute(HANDLE,WORD);
BOOL WINAPI SetConsoleOutputCP(UINT);

