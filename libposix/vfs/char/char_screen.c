/*
 * Copyright (c) 2016 Daniel Boland <dboland@xs4all.nl>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of its 
 *    contributors may be used to endorse or promote products derived 
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS 
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <wincon.h>

/****************************************************/

BOOL 
screen_TIOCGWINSZ(HANDLE Handle, WIN_WINSIZE *Result)
{
	CONSOLE_SCREEN_BUFFER_INFO sbInfo;
	BOOL bResult = FALSE;

	if (!GetConsoleScreenBufferInfo(Handle, &sbInfo)){
		WIN_ERR("GetConsoleScreenBufferInfo(%d): %s\n", Handle, win_strerror(GetLastError()));
	}else{
		Result->Column = sbInfo.dwSize.X;
		Result->Row = (sbInfo.srWindow.Bottom - sbInfo.srWindow.Top) + 1;
		Result->XPixel = sbInfo.dwCursorPosition.X + 1;
		Result->YPixel = sbInfo.dwCursorPosition.Y + 1;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
screen_TIOCSWINSZ(HANDLE Handle, WIN_WINSIZE *WinSize)
{
	BOOL bResult = FALSE;
	COORD cSize = {WinSize->Column, WinSize->Row};
	SMALL_RECT sRect = {0, 0, cSize.X - 1, cSize.Y - 1};

	if (!SetConsoleScreenBufferSize(Handle, cSize)){
		WIN_ERR("SetConsoleScreenBufferSize(%d): %s\n", Handle, win_strerror(GetLastError()));
	}else if (!SetConsoleWindowInfo(Handle, TRUE, &sRect)){
		WIN_ERR("SetConsoleWindowInfo(%d): %s\n", Handle, win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}

/****************************************************/

BOOL 
screen_poll(HANDLE Handle, WIN_POLLFD *Info, DWORD *Result)
{
	BOOL bResult = TRUE;
	SHORT sResult = WIN_POLLOUT;	/* ssh.exe */
	SHORT sMask = Info->Events | WIN_POLLIGNORE;

	if (Info->Result |= sResult & sMask){
		*Result += 1;
	}
	return(bResult);
}
