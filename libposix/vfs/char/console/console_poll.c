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

SHORT 
ConPollKey(HANDLE Handle, INPUT_RECORD *Record)
{
	SHORT sResult = 0;
	KEY_EVENT_RECORD *Event = &Record->KeyEvent;
	WORD VK = Event->wVirtualKeyCode;
	CHAR CH = Event->uChar.AsciiChar;
	BOOL bIsAnsi = FALSE;
	DWORD dwCount;

	if (!Event->bKeyDown){
		bIsAnsi = FALSE;
	}else if (CH){
		bIsAnsi = TRUE;
	}else if (VK <= VK_MODIFY){
		bIsAnsi = FALSE;
	}else if (VK <= VK_CURSOR){
		bIsAnsi = *ANSI_CURSOR(VK);
	}else if (VK <= VK_WINDOWS){
		bIsAnsi = FALSE;
	}else if (VK <= VK_NUMPAD){
		bIsAnsi = FALSE;
	}else if (VK <= VK_FUNCTION){
		bIsAnsi = *ANSI_FUNCTION(VK);
	}
	if (bIsAnsi){
		sResult = WIN_POLLIN;
	}else{
		ReadConsoleInput(Handle, Record, 1, &dwCount);
	}
	return(sResult);
}
SHORT 
ConPollBufferSize(HANDLE Handle, INPUT_RECORD *Record, CONSOLE_SCREEN_BUFFER_INFO *Info)
{
	SHORT sResult = 0;
	WINDOW_BUFFER_SIZE_RECORD *pbsEvent = &Record->WindowBufferSizeEvent;
	DWORD dwSize1 = *(DWORD *)&pbsEvent->dwSize;
	DWORD dwSize2 = *(DWORD *)&Info->dwSize;
	DWORD dwCount;

	/* When the Vista Console is in VIRTUAL_TERMINAL_PROCESSING (xterm)
	 * mode, multiple WINDOW_BUFFER_SIZE_EVENT are sent because of
	 * screen alternation.
	 */
	if (dwSize1 != dwSize2){
		Info->dwSize = pbsEvent->dwSize;
		sResult = WIN_POLLIN;
	}else{
		ReadConsoleInput(Handle, Record, 1, &dwCount);
	}
	return(sResult);
}
SHORT 
ConPollMouse(HANDLE Handle, INPUT_RECORD *Record)
{
	SHORT sResult = 0;
	MOUSE_EVENT_RECORD *pmEvent = &Record->MouseEvent;
	DWORD dwCount;

	if (pmEvent->dwEventFlags == MOUSE_WHEELED){
		sResult = WIN_POLLIN;
	}else{
		ReadConsoleInput(Handle, Record, 1, &dwCount);
	}
	return(sResult);
}
BOOL 
ConPollEvent(HANDLE Handle, INPUT_RECORD *Record, SHORT *Result)
{
	BOOL bResult = TRUE;
	DWORD dwCount;
	SHORT sResult = 0;

	switch (Record->EventType){
		case KEY_EVENT:
			sResult = ConPollKey(Handle, Record);
			break;
		case WINDOW_BUFFER_SIZE_EVENT:
			sResult = ConPollBufferSize(Handle, Record, &__CTTY->Info);
			break;
		case MOUSE_EVENT:
			sResult = ConPollMouse(Handle, Record);
			break;
		case FOCUS_EVENT:
		case MENU_EVENT:
			bResult = ReadConsoleInput(Handle, Record, 1, &dwCount);
			break;
		default:
			sResult = WIN_POLLERR;
			SetLastError(ERROR_IO_DEVICE);
	}
	*Result = sResult;
	return(bResult);
}

/****************************************************/

BOOL 
con_poll(WIN_TTY *Terminal, WIN_POLLFD *Info, DWORD *Result)
{
	BOOL bResult = TRUE;
	DWORD dwCount = 0;
	INPUT_RECORD iRecord;
	SHORT sResult = WIN_POLLOUT;
	SHORT sMask = Info->Events | WIN_POLLIGNORE;

	if (*__Input || __Clipboard){		/* vim.exe */
		sResult = WIN_POLLIN;
	}else if (!PeekConsoleInput(Terminal->Input, &iRecord, 1, &dwCount)){
		bResult = FALSE;
	}else if (dwCount){
		bResult = ConPollEvent(Terminal->Input, &iRecord, &sResult);
	}
	if (Info->Result |= sResult & sMask){
		*Result += 1;
	}
	return(bResult);
}
