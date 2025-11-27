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

#include <winbase.h>

/****************************************************/

BOOL 
char_TIOCGWINSZ(WIN_VNODE *Node, WIN_WINSIZE *WinSize)
{
	BOOL bResult = FALSE;

	switch (Node->DeviceType){
		case DEV_TYPE_SCREEN:
			bResult = screen_TIOCGWINSZ(Node->Handle, WinSize);
			break;
		default:
			SetLastError(ERROR_BAD_DEVICE);
	}
	return(bResult);
}
BOOL 
char_TIOCSWINSZ(WIN_VNODE *Node, WIN_WINSIZE *WinSize)
{
	BOOL bResult = FALSE;

	switch (Node->DeviceType){
		case DEV_TYPE_SCREEN:
			bResult = screen_TIOCSWINSZ(Node->Handle, WinSize);
			break;
		default:
			SetLastError(ERROR_BAD_DEVICE);
	}
	return(bResult);
}
BOOL 
char_TIOCSETA(WIN_VNODE *Node, WIN_TERMIO *Attribs)
{
	BOOL bResult = FALSE;

	switch (Node->DeviceType){
		case DEV_TYPE_INPUT:
			bResult = SetConsoleMode(Node->Handle, InputMode(Attribs));
			break;
		case DEV_TYPE_SCREEN:
			bResult = SetConsoleMode(Node->Handle, ScreenMode(Attribs));
			break;
		default:
			SetLastError(ERROR_BAD_DEVICE);
	}
	return(TRUE);
}
BOOL 
char_TIOCFLUSH(WIN_VNODE *Node)
{
	BOOL bResult = FALSE;

	switch (Node->DeviceType){
		case DEV_TYPE_INPUT:
			bResult = input_TIOCFLUSH(Node->Handle);
			break;
		default:
			SetLastError(ERROR_BAD_DEVICE);
	}
	return(bResult);
}
BOOL 
char_TIOCDRAIN(WIN_VNODE *Node)
{
	BOOL bResult = FALSE;

	switch (Node->DeviceType){
		case DEV_TYPE_SCREEN:
			bResult = screen_TIOCDRAIN(Node->Handle);
			break;
		default:
			SetLastError(ERROR_BAD_DEVICE);
	}
	return(bResult);
}
BOOL 
char_TIOCSCTTY(WIN_DEVICE *Device, WIN_TTY *Terminal)
{
	WIN_FLAGS wFlags = {GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0};
	SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};
	DWORD dwMode;

	Device->Input = CharOpenFile("CONIN$", &wFlags, &sa);
	Device->Output = CharOpenFile("CONOUT$", &wFlags, &sa);
	Device->Event = Device->Input;
	Device->Index = Terminal->Index;
	SetConsoleTextAttribute(Device->Output, BACKGROUND_BLACK | FOREGROUND_WHITE);
	GetConsoleScreenBufferInfo(Device->Output, &Terminal->Info);
	Terminal->DeviceType = Device->DeviceType;
	Terminal->DeviceId = Device->DeviceId;
	Terminal->Event = Device->Input;
//	Terminal->Flags = TIOCFLAG_ACTIVE;
	SetConsoleOutputCP(CP_UTF8);
	return(TRUE);
}
