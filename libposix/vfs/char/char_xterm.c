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

/* By setting the 'VirtualTerminalLevel' registry key to a DWORD value greater
 * than 1, the Vista Console goes into full XTerm mode. This gets you:
 * Overall faster screen drawing;
 * Underlined terms in man pages;
 * Beautiful mouse scrolling in vim;
 * Colour coded file listings on remote Linux systems;
 * The alternate screen feature (MS calls it 'application mode');
 */

/****************************************************/

DWORD 
XTermScreenMode(WIN_TERMIO *Attribs)
{
	DWORD dwResult = 0;
	UINT uiFlags = WIN_OPOST | WIN_ONLCR;

	if ((Attribs->OFlags & uiFlags) != uiFlags){
		dwResult |= DISABLE_NEWLINE_AUTO_RETURN;
	}
	return(dwResult);
}
SHORT 
XTermPollKey(HANDLE Handle, INPUT_RECORD *Record)
{
	SHORT sResult = 0;
	KEY_EVENT_RECORD *pkEvent = &Record->KeyEvent;
	DWORD dwCount;

	if (pkEvent->bKeyDown){
		sResult = WIN_POLLIN;
	}else{
		ReadConsoleInput(Handle, Record, 1, &dwCount);
	}
	return(sResult);
}
BOOL 
XTermPollEvent(HANDLE Handle, INPUT_RECORD *Record, SHORT *Result)
{
	BOOL bResult = TRUE;
	DWORD dwCount;
	SHORT sResult = 0;

	switch (Record->EventType){
		case KEY_EVENT:
			sResult = XTermPollKey(Handle, Record);
			break;
		case MOUSE_EVENT:
			sResult = WIN_POLLIN;
			break;
		case WINDOW_BUFFER_SIZE_EVENT:
//			bResult = InputBufferSize(&Record->WindowBufferSizeEvent);
//			break;
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
