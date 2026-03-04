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

/* The Vista Console can go into full XTerm mode. This gets you:
 * Overall faster screen drawing;
 * Underlined terms in man pages;
 * Beautiful mouse scrolling in vim;
 * Colour coded file listings on remote Linux systems;
 * The alternate screen feature (MS calls it 'application mode');
 */

/****************************************************/

SHORT 
InputPollKey(HANDLE Handle, INPUT_RECORD *Record)
{
	SHORT sResult = 0;
	KEY_EVENT_RECORD *pkEvent = &Record->KeyEvent;
	DWORD dwCount;

	if (!pkEvent->bKeyDown){
		ReadConsoleInput(Handle, Record, 1, &dwCount);
	}else if (pkEvent->uChar.AsciiChar){
		sResult = WIN_POLLIN;
	}else{
		ReadConsoleInput(Handle, Record, 1, &dwCount);
	}
	return(sResult);
}
BOOL 
InputPollEvent(HANDLE Handle, INPUT_RECORD *Record, SHORT *Result)
{
	BOOL bResult = TRUE;
	DWORD dwCount;
	SHORT sResult = WIN_POLLERR;

	switch (Record->EventType){
		case KEY_EVENT:
			sResult = InputPollKey(Handle, Record);
			break;
		case MOUSE_EVENT:
			sResult = WIN_POLLIN;
			break;
		case WINDOW_BUFFER_SIZE_EVENT:
		case FOCUS_EVENT:
		case MENU_EVENT:
			sResult = 0;
			bResult = ReadConsoleInput(Handle, Record, 1, &dwCount);
			break;
		default:
			SetLastError(ERROR_IO_DEVICE);
	}
	*Result = sResult;
	return(bResult);
}

/****************************************************/

BOOL 
input_poll(HANDLE Handle, WIN_POLLFD *Info, DWORD *Result)
{
	BOOL bResult = TRUE;
	DWORD dwCount = 0;
	INPUT_RECORD iRecord;
	SHORT sResult = WIN_POLLOUT;
	SHORT sMask = Info->Events | WIN_POLLIGNORE;

	if (!PeekConsoleInput(Handle, &iRecord, 1, &dwCount)){
		bResult = FALSE;
	}else if (dwCount){
		bResult = InputPollEvent(Handle, &iRecord, &sResult);
	}
	if (Info->Result |= sResult & sMask){
		*Result += 1;
	}
	return(bResult);
}
