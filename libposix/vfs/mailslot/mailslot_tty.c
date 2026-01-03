/*
 * Copyright (c) 2025 Daniel Boland <dboland@xs4all.nl>.
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

#include <winbase.h>

/****************************************************/

BOOL 
TTYGetString(HANDLE Handle, LPSTR Buffer)
{
	BOOL bResult = FALSE;
	DWORD dwResult = 0;

	*Buffer = 0;
	if (ReadFile(Handle, Buffer, WIN_MAX_INPUT, &dwResult, NULL)){
		Buffer[dwResult] = 0;
		bResult = TRUE;
	}
	__Input = Buffer;
	return(bResult);
}
BOOL 
TTYPutString(DWORD Owner, HANDLE Handle, LPCSTR Buffer, DWORD Size, OVERLAPPED *Ovl)
{
	BOOL bResult = TRUE;
	LONG lSize = Size;
	DWORD dwCount;

	while (lSize > 0){
		if (lSize < WIN_MAX_INPUT){
			dwCount = lSize;
		}else{
			dwCount = WIN_MAX_INPUT;
		}
		if (!WriteFile(Handle, Buffer, dwCount, &dwCount, Ovl)){
			bResult = FALSE;
			break;
		}else{
			win_kill(Owner, WM_APP, 0, 0);
			lSize -= dwCount;
			Buffer += dwCount;
		}
	}
	return(bResult);
}
BOOL 
TTYLineFeed(DWORD Owner, HANDLE Handle, WIN_TERMIO *Attribs, OVERLAPPED *Ovl)
{
	UINT oFlags = Attribs->OFlags;
	UINT lFlags = Attribs->LFlags;
	UINT uiFlags = WIN_OPOST | WIN_ONLCR;
	DWORD dwCount;
	BOOL bResult;

	if ((oFlags & uiFlags) == uiFlags){		/* ssh.exe */
		bResult = WriteFile(Handle, "\r\n", 2, &dwCount, Ovl);
	}else{
		bResult = WriteFile(Handle, "\n", 1, &dwCount, Ovl);
	}
	win_kill(Owner, WM_APP, 0, 0);
	return(bResult);
}
BOOL 
TTYCarriageReturn(DWORD Owner, HANDLE Handle, WIN_TERMIO *Attribs, OVERLAPPED *Ovl)
{
	UINT oFlags = Attribs->OFlags;
	UINT lFlags = Attribs->LFlags;
	UINT uiFlags = WIN_OPOST | WIN_OCRNL;
	BOOL bResult;
	DWORD dwCount;

	if ((oFlags & uiFlags) == uiFlags){
		bResult = WriteFile(Handle, "\n", 1, &dwCount, Ovl);
	}else{
		bResult = WriteFile(Handle, "\r", 1, &dwCount, Ovl);
	}
	win_kill(Owner, WM_APP, 0, 0);
	return(bResult);
}

/****************************************************/

BOOL 
tty_read(WIN_TASK *Task, WIN_TTY *Terminal, LPSTR Buffer, DWORD Size, DWORD *Result)
{
	BOOL bResult = FALSE;
	CHAR C;
	DWORD dwResult = 0;
	LONG lSize = Size;
	HANDLE hInput = Terminal->Input;
	WIN_TERMIO *pwAttribs = &Terminal->Attribs;

	while (!bResult){
		if (lSize < 1){
			bResult = TRUE;
		}else if (C = *__Input){
			*Buffer++ = C;
			dwResult++;
			lSize--;
			__Input++;
		}else if (dwResult){
			bResult = TRUE;
		}else if (!TTYGetString(hInput, __INPUT_BUF)){
			break;
		}else if (proc_poll(Task)){
			break;
		}
	}
	*Result = dwResult;
	return(bResult);
}
BOOL 
tty_write(WIN_TTY *Terminal, LPCSTR Buffer, DWORD Size, DWORD *Result)
{
	BOOL bResult = TRUE;
	OVERLAPPED ovl = {0, 0, 0, 0, Terminal->Event};
	DWORD dwCount = 0;
	HANDLE hOutput = Terminal->Output;
	LONG lSize = Size;
	DWORD dwResult = 0;
	WIN_TERMIO *pwAttribs = &Terminal->Attribs;
	DWORD dwOwner = Terminal->ThreadId;
	CHAR C;

	while (lSize > 0){
		C = Buffer[dwCount];
		if (C < 16){
			TTYPutString(dwOwner, hOutput, Buffer, dwCount, &ovl);
			Buffer += dwCount;
			dwCount = 0;
			if (C == CC_LF){
				TTYLineFeed(dwOwner, hOutput, pwAttribs, &ovl);
				Buffer++;
			}else if (C == CC_CR){
				TTYCarriageReturn(dwOwner, hOutput, pwAttribs, &ovl);
				Buffer++;
			}else{
				dwCount++;
			}
		}else{
			dwCount++;
		}
		dwResult++;
		lSize--;
	}
	if (!TTYPutString(dwOwner, hOutput, Buffer, dwCount, &ovl)){
		bResult = FALSE;
	}
	*Result = dwResult;
	return(bResult);
}
BOOL 
tty_poll(WIN_TTY *Terminal, WIN_POLLFD *Info, DWORD *Result)
{
	BOOL bResult = TRUE;
	SHORT sResult = WIN_POLLERR;
	SHORT sMask = Info->Events | WIN_POLLIGNORE;
	DWORD dwSize = 0;
	DWORD dwCount = 0;

	if (*__Input || __Clipboard){		/* nano.exe */
		sResult = WIN_POLLIN;
	}else if (!GetMailslotInfo(Terminal->Input, NULL, &dwSize, &dwCount, NULL)){
		bResult = FALSE;
	}else if (dwSize == MAILSLOT_NO_MESSAGE){
		sResult = WIN_POLLOUT;
	}else if (dwSize){
		sResult = WIN_POLLOUT | WIN_POLLIN;
	}else{
		sResult = 0;
	}
	if (Info->Result = sMask & sResult){
		*Result += 1;
	}
	return(bResult);
}
