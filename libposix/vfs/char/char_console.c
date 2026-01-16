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
		if (vfs_raise(WM_SIGNAL, CtrlType, 0)){
			SetEvent(__Interrupt);		/* ping6.exe */
		}
	}
	return(bResult);
}

/****************************************************/

BOOL 
con_TIOCSCTTY(WIN_DEVICE *Device, WIN_TASK *Task, WIN_TTY *Terminal)
{
	WIN_FLAGS wFlags = {GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0};
	SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};
	DWORD *dwMode;

	if (!Terminal){
		return(FALSE);
	}else{
		dwMode = Terminal->Mode;
	}
	Terminal->Input = GetStdHandle(STD_INPUT_HANDLE);
	Terminal->Output = GetStdHandle(STD_OUTPUT_HANDLE);
	Terminal->FSType = FS_TYPE_CHAR;
	Terminal->SessionId = Task->SessionId;
	Terminal->GroupId = Task->GroupId;
	SetConsoleTextAttribute(Terminal->Output, BACKGROUND_BLACK | FOREGROUND_WHITE);
	GetConsoleScreenBufferInfo(Terminal->Output, &Terminal->Info);
	SetConsoleOutputCP(CP_UTF8);
	Device->FSType = FS_TYPE_CHAR;
	Device->Event = Terminal->Input;
	Task->Flags |= WIN_PS_CONTROLT;
	Task->TerminalId = Terminal->Index;
	GetConsoleMode(Terminal->Input, &dwMode[0]);
	GetConsoleMode(Terminal->Output, &dwMode[1]);
//	vfs_ktrace(L"con_TIOCSCTTY", STRUCT_TTY, Terminal);
	return(TRUE);
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

	if (!SetConsoleMode(Terminal->Input, InputMode(Terminal->Mode[0], Attribs))){
		WIN_ERR("SetConsoleMode(%d): %s\n", Terminal->Input, win_strerror(GetLastError()));
	}else if (!SetConsoleMode(Terminal->Output, ScreenMode(Terminal->Mode[1], Attribs))){
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
		Device->FSType = FS_TYPE_PDO;
		Device->Flags = 0;
		bResult = TRUE;
	}
	return(bResult);
}
