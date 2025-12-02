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

BOOL CALLBACK 
ConControlHandler(DWORD CtrlType)
{
	BOOL bResult = TRUE;

	/* To deliver signals, Windows (CSRSS.EXE) actually forks!
	 * Copying the call stack to a new thread and executing
	 * our code. Let's make sure it uses our Task struct too:
	 */
	TlsSetValue(__TlsIndex, (PVOID)__Process->TaskId);
	if (__Process->GroupId == __CTTY->GroupId){
		if (vfs_raise(WM_COMMAND, CtrlType, 0)){
			SetEvent(__Interrupt);		/* ping6.exe */
		}
	}
	return(bResult);
}

/****************************************************/

BOOL 
con_TIOCSCTTY(WIN_DEVICE *Device, WIN_TTY **Result)
{
	BOOL bResult = FALSE;
	WIN_FLAGS wFlags = {GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0};
	SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};
	WIN_TTY *pwTerminal = NULL;

	if (pwTerminal = tty_attach()){
		pwTerminal->Input = CharOpenFile("CONIN$", &wFlags, &sa);
		pwTerminal->Output = CharOpenFile("CONOUT$", &wFlags, &sa);
		pwTerminal->Event = pwTerminal->Input;
		pwTerminal->FSType = FS_TYPE_CHAR;
		pwTerminal->DeviceType = Device->DeviceType;
		pwTerminal->DeviceId = Device->DeviceId;
		SetConsoleTextAttribute(pwTerminal->Output, BACKGROUND_BLACK | FOREGROUND_WHITE);
		GetConsoleScreenBufferInfo(pwTerminal->Output, &pwTerminal->Info);
		Device->FSType = FS_TYPE_CHAR;
		Device->Index = pwTerminal->Index;
//pwTerminal->Flags = TIOCFLAG_ACTIVE;
		*Result = pwTerminal;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
con_TIOCGWINSZ(WIN_TTY *Terminal, WIN_WINSIZE *WinSize)
{
	return(screen_TIOCGWINSZ(Terminal->Output, WinSize));
}
BOOL 
con_TIOCSWINSZ(WIN_TTY *Terminal, WIN_WINSIZE *WinSize)
{
	return(screen_TIOCSWINSZ(Terminal->Output, WinSize));
}
BOOL 
con_TIOCSETA(WIN_TTY *Terminal, WIN_TERMIO *Attribs)
{
	BOOL bResult = FALSE;

	if (!SetConsoleMode(Terminal->Input, InputMode(Attribs))){
		WIN_ERR("SetConsoleMode(%d): %s\n", Terminal->Input, win_strerror(GetLastError()));
	}else if (!SetConsoleMode(Terminal->Output, ScreenMode(Attribs))){
		WIN_ERR("SetConsoleMode(%d): %s\n", Terminal->Output, win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}

/****************************************************/

HANDLE 
con_F_OSFHANDLE(WIN_TTY *Terminal, DWORD Index)
{
	if (!Index){
		return(Terminal->Input);
	}else{
		return(Terminal->Output);
	}
}

/****************************************************/

BOOL 
con_fsync(WIN_TTY *Terminal)
{
	return(FlushConsoleInputBuffer(Terminal->Input));
}
BOOL 
con_poll(WIN_TTY *Terminal, WIN_POLLFD *Info, DWORD *Result)
{
	DWORD dwResult = 0;

	if (!input_poll(Terminal->Input, Info, &dwResult)){
		return(FALSE);
	}
	if (!screen_poll(Terminal->Output, Info, &dwResult)){
		return(FALSE);
	}
	if (dwResult){
		*Result += 1;
	}
	return(TRUE);
}
BOOL 
con_revoke(WIN_TTY *Terminal, WIN_DEVICE *Device)
{
	BOOL bResult = FALSE;

	if (!CloseHandle(Terminal->Output)){
		WIN_ERR("CloseHandle(%d): %s\n", Terminal->Output, win_strerror(GetLastError()));
	}else if (!CloseHandle(Terminal->Input)){
		WIN_ERR("CloseHandle(%d): %s\n", Terminal->Input, win_strerror(GetLastError()));
	}else{
		Terminal->Input = NULL;
		Terminal->Output = NULL;
		Terminal->Flags = 0;
		Device->Flags = 0;
		Device->FSType = FS_TYPE_PDO;
		bResult = TRUE;
	}
	return(bResult);
}
