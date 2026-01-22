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

#define CON_MODE_SCREEN	ENABLE_PROCESSED_OUTPUT

/****************************************************/

DWORD 
ScreenMode(DWORD Mode, WIN_TERMIO *Attribs)
{
	DWORD dwResult = Mode & ~(CON_MODE_SCREEN);
	UINT uiFlags = WIN_OPOST | WIN_ONLCR;

	if (Attribs->OFlags & WIN_OXTABS){
		dwResult |= ENABLE_PROCESSED_OUTPUT;
	}
	if (Mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING){
		if ((Attribs->OFlags & uiFlags) != uiFlags){
			dwResult |= DISABLE_NEWLINE_AUTO_RETURN;
		}
	}
	return(dwResult);
}
BOOL 
ScreenRenderWindow(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info)
{
	BOOL bResult = FALSE;

	if (GetConsoleScreenBufferInfo(Handle, Info)){
		Info->srWindow.Left = 0;
		Info->srWindow.Right = Info->dwSize.X - 1;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
ScreenCarriageReturn(HANDLE Handle, UINT Flags, CONSOLE_SCREEN_BUFFER_INFO *Info)
{
	COORD cPos = Info->dwCursorPosition;
	UINT uiFlags = WIN_OPOST | WIN_OCRNL;

	if ((Flags & uiFlags) == uiFlags){
		cPos.Y++;
	}else{
		cPos.X = 0;
	}
	Info->dwCursorPosition = cPos;
	return(SetConsoleCursorPosition(Handle, cPos));
}
BOOL 
ScreenScrollUp(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info)
{
	SMALL_RECT sRect = Info->srWindow;
	COORD cPos = {0, 0};
	CHAR_INFO cInfo = {' ', Info->wAttributes & 0xFF};

	sRect.Top = 1;
	sRect.Bottom = Info->dwSize.Y - 1;
	return(ScrollConsoleScreenBuffer(Handle, &sRect, NULL, cPos, &cInfo));
}
BOOL 
ScreenLineFeed(HANDLE Handle, UINT Flags, CONSOLE_SCREEN_BUFFER_INFO *Info)
{
	SMALL_RECT sRect = Info->srWindow;
	COORD cPos = Info->dwCursorPosition;
	UINT uiFlags = WIN_OPOST | WIN_ONLCR;
	SHORT sBottom = Info->dwSize.Y - 1;

	if ((Flags & uiFlags) == uiFlags){		/* ssh.exe */
		cPos.X = 0;
	}
	if (cPos.Y < sRect.Bottom){
		cPos.Y++;
	}else if (__CTTY->VEdit){
		AnsiScrollUp(Handle, Info, 1);
	}else if (cPos.Y == sBottom){
		ScreenScrollUp(Handle, Info);
	}else{
		cPos.Y++;
		Info->srWindow.Top++;
		Info->srWindow.Bottom++;
	}
	Info->dwCursorPosition = cPos;
	return(SetConsoleCursorPosition(Handle, cPos));
}
BOOL 
ScreenAnsi(HANDLE Handle, CHAR C, SEQUENCE *Seq, CONSOLE_SCREEN_BUFFER_INFO *Info)
{
	BOOL bResult = FALSE;
	DWORD dwSize = __Escape - Seq->Buf;
	COORD cPos = Info->dwCursorPosition;

	if (Seq->CSI == '['){
		bResult = AnsiControl(Handle, C, Info, Seq, dwSize);
	}else if (Seq->CSI == '?'){		/* DEC private */
		if (C == 'h'){			/* DECSM */
			bResult = DECSetMode(Handle, AnsiStrToInt(Seq->Args));
		}else if (C == 'l'){		/* DECRM */
			bResult = DECResetMode(Handle, AnsiStrToInt(Seq->Args));
		}else if (C == 'u'){		/* DECRSPS */
			bResult = DECRestorePresentationState(Handle, Info, AnsiStrToInt(Seq->Args));
		}
	}else if (!Seq->CSI){			/* DEC private (ANSI allowed) */
		if (C == '7'){			/* DECSC (apt-get) */
			bResult = AnsiSaveCursor(Info);
		}else if (C == '8'){		/* DECRC (apt-get) */
			bResult = AnsiRestoreCursor(Handle, Info);
//		}else if (C == 'c'){		/* RIS */
//			bResult = AnsiResetToInitalState(Handle, Info);
		}
	}else if (Seq->CSI == '('){		/* DEC SCS (Shift-In) */
		bResult = DECSelectCharacterSet(Handle, C);
	}else if (Seq->CSI == ')'){		/* DEC SCS (Shift-Out) */
		bResult = DECResetCharacterSet(Handle, C);
	}
	if (!bResult){
		Seq->Buf[dwSize] = 0;
		WIN_ERR("\e%s", Seq->Buf);
	}
	__Escape = NULL;
	return(bResult);
}
VOID 
ScreenEscape(HANDLE Handle, CHAR C, SEQUENCE *Seq, CONSOLE_SCREEN_BUFFER_INFO *Info)
{
	*__Escape++ = C;

	if (C == '\a'){			/* end of hyperlink */
		__Escape = NULL;
	}else if (C == '@'){
		ScreenAnsi(Handle, C, Seq, Info);
	}else if (C == '_'){		/* invalid (Synchronet BBS probes) */
		__Escape = NULL;
	}else if (Seq->CSI == ']'){	/* hyperlinks (systemctl) */
		return;			/* ignore */
	}else if (C == ';'){
		Seq->Arg1 = AnsiStrToInt(Seq->Args);
		Seq->Char1 = 0;
		Seq->Args = __Escape;
	}else if (!IsCharAlphaNumeric(C)){
		Seq->Args++;
		Seq->CSI = C;
	}else if (IsCharAlpha(C)){
		ScreenAnsi(Handle, C, Seq, Info);
	}else if (!Seq->CSI){		/* no sequence introducer (apt-get) */
		ScreenAnsi(Handle, C, Seq, Info);
	}else if (!Seq->Char1){
		Seq->Char1 = C;
	}
}
DWORD 
ScreenPutString(HANDLE Handle, LPCSTR Buffer, DWORD Count, CONSOLE_SCREEN_BUFFER_INFO *Info)
{
	DWORD dwWidth = 0;

	/* On Vista, when emitting multibyte (ansi/utf8),
	 * dwWidth will be zero.
	 */
	if (WriteFile(Handle, Buffer, Count, &dwWidth, NULL)){
		ScreenRenderWindow(Handle, Info);
	}
	return(Count);
}

/****************************************************/

BOOL 
con_write(WIN_TTY *Terminal, LPCSTR Buffer, DWORD Size, DWORD *Result)
{
	BOOL bResult = TRUE;
	LONG lSize = Size;
	DWORD dwCount = 0;
	DWORD dwResult = 0;
	WIN_TERMIO *pwAttribs = &Terminal->Attribs;
	UINT uiOutFlags = pwAttribs->OFlags;
	UINT uiInFlags = pwAttribs->IFlags;
	CONSOLE_SCREEN_BUFFER_INFO *psbInfo = &Terminal->Info;
	HANDLE hOutput = Terminal->Output;

	if (!ScreenRenderWindow(hOutput, psbInfo)){
		return(FALSE);
	}else while (lSize > 0){
		__Char = Buffer[dwCount];
		if (__Escape){
			ScreenEscape(hOutput, __Char, &__ANSI_BUF, psbInfo);
			Buffer++;
			dwCount = 0;
		}else if (__Char < 32){
			/* Hello David Cuttler */
			ScreenPutString(hOutput, Buffer, dwCount, psbInfo);
			Buffer += dwCount;
			dwCount = 0;
			if (!__Char){			/* NULL Filler */
				Buffer++;
			}else if (__Char == CC_SOH){	/* Raspbian telnet */
				Buffer++;
			}else if (__Char == CC_ESC){
				__Escape = __ANSI_BUF.Buf;
				__ANSI_BUF.Arg1 = 1;
				__ANSI_BUF.Char1 = 0;
				__ANSI_BUF.CSI = 0;
				__ANSI_BUF.Args = __Escape;
				Buffer++;
			}else if (__Char == CC_BS){
				AnsiCursorBack(hOutput, psbInfo, 1);
				Buffer++;
			}else if (__Char == CC_LF){
				ScreenLineFeed(hOutput, uiOutFlags, psbInfo);
				Buffer++;
			}else if (__Char == CC_CR){
				ScreenCarriageReturn(hOutput, uiOutFlags, psbInfo);
				Buffer++;
			}else if (__Char == CC_SO){	/* Switch to alternative character set */
				SetConsoleOutputCP(GetACP());
				Buffer++;
			}else if (__Char == CC_SI){	/* Return to regular character set */
				SetConsoleOutputCP(CP_UTF8);
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
	if (dwCount){
		ScreenPutString(hOutput, Buffer, dwCount, psbInfo);
	}
	*Result = dwResult;
	return(bResult);
}
