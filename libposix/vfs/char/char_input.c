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

#define CC_NUL	0x00	/* Ignore */
#define CC_SOH	0x01	/* Ctrl+A (Start of Header) */
#define CC_ETX	0x03	/* Ctrl+C (End of Text) */
#define CC_EOT	0x04	/* Ctrl+D (End of Transmission) */
#define CC_BS	0x08	/* Ctrl+H (Back Space) */
#define CC_LF	0x0A	/* Ctrl+J (Line Feed) */
#define CC_CR	0x0D	/* Ctrl+M (Carriage Return) */
#define CC_SO	0x0E	/* Ctrl+N (Shift In) */
#define CC_SI	0x0F	/* Ctrl+O (Shift Out) */
#define CC_DC1	0x11	/* Ctrl+Q (Device Control 1: XON) */
#define CC_DC3	0x13	/* Ctrl+S (Device Control 3: XOFF) */
#define CC_ESC	0x1B	/* Escape */
#define CC_FS	0x1C	/* Ctrl+\ */

/****************************************************/

DWORD 
InputMode(WIN_TERMIO *Attribs)
{
	DWORD dwResult = __ConMode[0] | ENABLE_WINDOW_INPUT | ENABLE_INSERT_MODE | 
		ENABLE_QUICK_EDIT_MODE | ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT;

	if (Attribs->LFlags & WIN_ECHO){
		dwResult |= ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT;
	}else if (Attribs->LFlags & WIN_ICANON){
		dwResult |= ENABLE_LINE_INPUT;
	}
	if (Attribs->LFlags & WIN_ISIG){
		dwResult |= ENABLE_PROCESSED_INPUT;
	}
	return(dwResult);
}
BOOL 
InputChar(CHAR C, DWORD KeyState, CHAR *Buffer)
{
	DWORD dwCount;
	UINT uiMessage;

	if (KeyState & LEFT_ALT_PRESSED){		/* nano.exe */
		*Buffer++ = '\e';
	}
	*Buffer++ = C;
	*Buffer = 0;
	return(TRUE);
}
BOOL 
InputReturn(DWORD KeyState, WIN_TERMIO *Attribs, CHAR *Buffer)
{
	if (Attribs->IFlags & WIN_ICRNL){
		InputChar('\n', KeyState, Buffer);
	}else{
		InputChar('\r', KeyState, Buffer);
	}
	return(TRUE);
}
BOOL 
InputTabulator(DWORD KeyState, CHAR *Buffer)
{
	if (KeyState & SHIFT_PRESSED){
		win_strcpy(Buffer, "\e[Z");
	}else{
		InputChar('\t', KeyState, Buffer);
	}
	return(TRUE);
}
BOOL 
InputInsert(DWORD KeyState, CHAR *Buffer)
{
	BOOL bResult = TRUE;

	if (!(KeyState & SHIFT_PRESSED)){
		win_strcpy(Buffer, ANSI_CURSOR(VK_INSERT));
	}else if (!IsClipboardFormatAvailable(CF_TEXT)){
		*Buffer = 0;
	}else if (!OpenClipboard(NULL)){
		bResult = FALSE;
	}else if (__Lock = GetClipboardData(CF_UNICODETEXT)){
		__Clipboard = GlobalLock(__Lock);
	}else{
		bResult = FALSE;
	}
	return(bResult);
}
BOOL 
InputKey(KEY_EVENT_RECORD *Event, WIN_TERMIO *Attribs, CHAR *Buffer)
{
	BOOL bResult = TRUE;
	CHAR CH = Event->uChar.AsciiChar;
	WORD VK = Event->wVirtualKeyCode;
	DWORD dwState = Event->dwControlKeyState;

//WIN_ERR("[0x%x]", CH);
	if (!Event->bKeyDown){
		*Buffer = 0;
	}else if (VK == VK_RETURN){
		bResult = InputReturn(dwState, Attribs, Buffer);
	}else if (VK == VK_TAB){
		bResult = InputTabulator(dwState, Buffer);
//	}else if (CH == CC_ETX){
//		bResult = GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);
	}else if (CH){
		bResult = InputChar(CH, dwState, Buffer);
	}else if (VK == VK_INSERT){
		bResult = InputInsert(dwState, Buffer);
	}else if (VK <= VK_MODIFY){
		*Buffer = 0;
	}else if (VK <= VK_CURSOR){
		win_strcpy(Buffer, ANSI_CURSOR(VK));
	}else if (VK <= VK_WINDOWS){
		*Buffer = 0;
	}else if (VK <= VK_NUMPAD){
		*Buffer = 0;
	}else if (VK <= VK_FUNCTION){
		win_strcpy(Buffer, ANSI_FUNCTION(VK));
	}else{
		*Buffer = 0;
	}
	__Input = Buffer;
	return(bResult);
}
BOOL 
InputBufferSize(WINDOW_BUFFER_SIZE_RECORD *Event)
{
	BOOL bResult = TRUE;

	if (vfs_raise(WM_COMMAND, CTRL_SIZE_EVENT, 0)){
		bResult = FALSE;
	}else{
		SetEvent(__Interrupt);
	}
	return(bResult);
}
BOOL 
InputMouse(MOUSE_EVENT_RECORD *Event, CHAR *Buffer)
{
	if (Event->dwEventFlags != MOUSE_WHEELED){
		return(TRUE);
	}else if (GET_WHEEL_DELTA_WPARAM(Event->dwButtonState) > 0){
		win_strcpy(Buffer, ANSI_CURSOR(VK_UP));
	}else{
		win_strcpy(Buffer, ANSI_CURSOR(VK_DOWN));
	}
	return(TRUE);
}
BOOL 
InputReadEvent(HANDLE Handle, WIN_TERMIO *Attribs, CHAR *Buffer)
{
	BOOL bResult = FALSE;
	INPUT_RECORD iRecord;
	DWORD dwCount = 0;

	if (!ReadConsoleInput(Handle, &iRecord, 1, &dwCount)){
		WIN_ERR("ReadConsoleInput(%d): %s\n", Handle, win_strerror(GetLastError()));
	}else switch (iRecord.EventType){
		case KEY_EVENT:
			bResult = InputKey(&iRecord.KeyEvent, Attribs, Buffer);
			break;
		case WINDOW_BUFFER_SIZE_EVENT:
			bResult = InputBufferSize(&iRecord.WindowBufferSizeEvent);
			break;
		case FOCUS_EVENT:
		case MENU_EVENT:
		case MOUSE_EVENT:
			bResult = TRUE;
			break;
		default:
			SetLastError(ERROR_IO_DEVICE);
	}
	return(bResult);
}
BOOL 
InputReadLine(HANDLE Handle, WIN_TERMIO *Attribs, CHAR *Buffer)
{
	BOOL bResult = FALSE;
	LONG lCount = 0;

	*Buffer = 0;
	if (!ReadFile(Handle, Buffer, WIN_MAX_INPUT, &lCount, NULL)){
		WIN_ERR("ReadFile(%d): %s\n", Handle, win_strerror(GetLastError()));
	}else if (lCount > 0){
		lCount--;
		if (Attribs->LFlags & WIN_ISIG){	/* ENABLE_PROCESSED_INPUT */
			Buffer[lCount--] = 0;		/* remove NL, leave CR */
		}
		if (Attribs->IFlags & WIN_ICRNL){
			Buffer[lCount] = '\n';		/* replace CR (ftp.exe) */
		}
		bResult = TRUE;
	}
	__Input = Buffer;
	return(bResult);
}
BOOL 
InputReadClipboard(CHAR *Buffer)
{
	BOOL bResult = TRUE;
	WCHAR szBuffer[WIN_MAX_INPUT], *pszBuffer = szBuffer;
	LONG lSize = WIN_MAX_INPUT;
	WCHAR C;

	*Buffer = 0;
	while (lSize > 0){
		C = *__Clipboard;
		if (!C){
			GlobalUnlock(__Lock);
			CloseClipboard();
			__Clipboard = NULL;
			break;
		}else if (C != '\n'){
			*pszBuffer++ = C;
			lSize--;
		}
		__Clipboard++;
	}
	*pszBuffer = 0;
	win_wcstombs(Buffer, szBuffer, WIN_MAX_INPUT);
	__Input = Buffer;
	return(bResult);
}
BOOL 
InputWaitChar(HANDLE Handle, WIN_TERMIO *Attribs, LPSTR Buffer)
{
	BOOL bResult = FALSE;
	DWORD dwStatus;

	dwStatus = WaitForSingleObjectEx(Handle, INFINITE, TRUE);
	if (dwStatus == WAIT_FAILED){
		WIN_ERR("WaitForMultipleObjectsEx(%d): %s\n", Handle, win_strerror(GetLastError()));
	}else if (Attribs->LFlags & WIN_ICANON){		/* ENABLE_LINE_INPUT */
		bResult = InputReadLine(Handle, Attribs, Buffer);
	}else{
		bResult = InputReadEvent(Handle, Attribs, Buffer);
	}
	return(bResult);
}
SHORT 
InputPollAnsi(HANDLE Handle, INPUT_RECORD *Record)
{
	SHORT sResult = 0;
	KEY_EVENT_RECORD *Event = &Record->KeyEvent;
	WORD VK = Event->wVirtualKeyCode;
	BOOL bIsAnsi = FALSE;
	DWORD dwCount;

	if (!Event->bKeyDown){
		bIsAnsi = FALSE;
	}else if (Event->uChar.AsciiChar){
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
BOOL 
InputPollEvent(HANDLE Handle, INPUT_RECORD *Record, SHORT *Result)
{
	BOOL bResult = TRUE;
	DWORD dwCount;

	switch (Record->EventType){
		case KEY_EVENT:
			*Result = InputPollAnsi(Handle, Record);
			break;
		case WINDOW_BUFFER_SIZE_EVENT:
			*Result = WIN_POLLIN;
			break;
		case MOUSE_EVENT:
		case FOCUS_EVENT:
		case MENU_EVENT:
			bResult = ReadConsoleInput(Handle, Record, 1, &dwCount);
			break;
		default:
			*Result = WIN_POLLERR;
			SetLastError(ERROR_IO_DEVICE);
	}
	return(bResult);
}

/****************************************************/

BOOL 
input_read(WIN_TASK *Task, HANDLE Handle, LPSTR Buffer, DWORD Size, DWORD *Result)
{
	BOOL bResult = FALSE;
	CHAR C = 0;
	DWORD dwResult = 0;
	LONG lSize = Size;
	WIN_TERMIO *pwAttribs = &__CTTY->Attribs;

	if (__ConMode[0] & ENABLE_VIRTUAL_TERMINAL_INPUT){
		bResult = ReadFile(Handle, Buffer, Size, &dwResult, NULL);
	}else while (!bResult){
		if (lSize < 1){
			bResult = TRUE;
		}else if (C = *__Input){
			*Buffer++ = C;
			dwResult++;
			lSize--;
			__Input++;
		}else if (dwResult){
			__Index = 0;
			bResult = TRUE;
		}else if (__Clipboard){
			InputReadClipboard(__INPUT_BUF);
		}else if (!InputWaitChar(Handle, pwAttribs, __INPUT_BUF)){
			break;
		}else if (proc_poll(Task)){
			break;
		}
	}
	*Result = dwResult;
	return(bResult);
}
BOOL 
input_poll(HANDLE Handle, WIN_POLLFD *Info, DWORD *Result)
{
	BOOL bResult = TRUE;
	DWORD dwCount = 0;
	INPUT_RECORD iRecord;
	SHORT sResult = WIN_POLLERR;
	SHORT sMask = Info->Events | WIN_POLLIGNORE;

	if (*__Input || __Clipboard){		/* vim.exe */
		sResult = WIN_POLLIN;
	}else if (!PeekConsoleInput(Handle, &iRecord, 1, &dwCount)){
		bResult = FALSE;
	}else if (dwCount){
		bResult = InputPollEvent(Handle, &iRecord, &sResult);
	}else{
		sResult = 0;
	}
	if (Info->Result |= sResult & sMask){
		*Result += 1;
	}
	return(bResult);
}

/****************************************************/

BOOL 
input_TIOCFLUSH(HANDLE Handle)
{
	BOOL bResult = FALSE;

	/* "Handle is invalid" if CONIN$ buffer empty
	 */
	if (!FlushConsoleInputBuffer(Handle)){
		WIN_ERR("FlushConsoleInputBuffer(%d): %s\n", Handle, win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}
