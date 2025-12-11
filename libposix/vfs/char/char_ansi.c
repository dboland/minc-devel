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

/* Warning! Do NOT optimize this code by refactoring shared
 * function calls. Debugging becomes almost impossible.
 */

#include <wincon.h>

#define FOREGROUND_WHITE	(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
#define FOREGROUND_YELLOW	(FOREGROUND_RED | FOREGROUND_GREEN)
#define FOREGROUND_MAGENTA	(FOREGROUND_BLUE | FOREGROUND_RED)
#define FOREGROUND_CYAN		(FOREGROUND_BLUE | FOREGROUND_GREEN)
#define FOREGROUND_BLACK	(0x0)
#define FOREGROUND_DEFAULT	FOREGROUND_WHITE
#define FOREGROUND_UNDERLINE	FOREGROUND_CYAN

#define BACKGROUND_WHITE	(BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE)
#define BACKGROUND_YELLOW	(BACKGROUND_RED | BACKGROUND_GREEN)
#define BACKGROUND_MAGENTA	(BACKGROUND_BLUE | BACKGROUND_RED)
#define BACKGROUND_CYAN		(BACKGROUND_BLUE | BACKGROUND_GREEN)
#define BACKGROUND_BLACK	(0x0)
#define BACKGROUND_DEFAULT	BACKGROUND_BLACK

/****************************************************/

LONG 
AnsiStrToInt(LPCSTR String)
{
	LONG lResult = 0;

	if (!(lResult = atoi(String))){
		lResult = 1;
	}
	return(lResult);
}
BOOL 
AnsiScrollHorizontal(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, WORD DeltaLeft, WORD DeltaRight)
{
	SMALL_RECT sRect;
	CHAR_INFO cInfo = {' ', Info->wAttributes & 0xFF};
	COORD cPos = Info->dwCursorPosition;

	cPos.X += DeltaRight;
	sRect.Left = Info->dwCursorPosition.X + DeltaLeft;
	sRect.Right = Info->dwSize.X - DeltaRight;
	sRect.Top = Info->dwCursorPosition.Y;
	sRect.Bottom = Info->dwCursorPosition.Y;
	return(ScrollConsoleScreenBuffer(Handle, &sRect, NULL, cPos, &cInfo));
}
WORD 
AnsiInvertAttribs(CONSOLE_SCREEN_BUFFER_INFO *Info)
{
	WORD wBackGround = (Info->wAttributes & 0xF0) / 0x10;
	WORD wForeGround = (Info->wAttributes & 0x0F) * 0x10;

	return(wBackGround | wForeGround);
}
BOOL 
AnsiEqualRect(SMALL_RECT *Rect1, SMALL_RECT *Rect2)
{
	DWORDLONG dwlRect1 = *(DWORDLONG *)Rect1;
	DWORDLONG dwlRect2 = *(DWORDLONG *)Rect2;

	if (dwlRect1 != dwlRect2){
		return(FALSE);
	}
	return(TRUE);
}

/****************************************************/

BOOL 
AnsiRenderMargin(HANDLE Handle, WORD Top, WORD Bottom, SMALL_RECT *Result)
{
	BOOL bResult = FALSE;
	CONSOLE_SCREEN_BUFFER_INFO csbInfo;

	if (!GetConsoleScreenBufferInfo(Handle, &csbInfo)){
		WIN_ERR("GetConsoleScreenBufferInfo(%d): %s\n", Handle, win_strerror(GetLastError()));
	}else{
		*Result = csbInfo.srWindow;
		Result->Bottom = Result->Top + Bottom - 1;
		Result->Top += Top - 1;
		bResult = !AnsiEqualRect(Result, &csbInfo.srWindow);
	}
	return(bResult);
}
COORD 
AnsiRenderCursor(CONSOLE_SCREEN_BUFFER_INFO *Info)
{
	COORD cPos = Info->dwCursorPosition;

	if (cPos.X > Info->srWindow.Right){
		cPos.X -= Info->srWindow.Right + 1;
		cPos.Y++;
	}
	return(cPos);
}
BOOL 
AnsiRenderForeground(CONSOLE_SCREEN_BUFFER_INFO *Info, CHAR Char2)
{
	BOOL bResult = TRUE;
	WORD wAttrib = Info->wAttributes & 0xFFF8;

	if (Char2 == '0'){
		wAttrib |= FOREGROUND_BLACK;
	}else if (Char2 == '1'){
		wAttrib |= FOREGROUND_RED;
	}else if (Char2 == '2'){
		wAttrib |= FOREGROUND_GREEN;
	}else if (Char2 == '3'){
		wAttrib |= FOREGROUND_YELLOW;
	}else if (Char2 == '4'){
		wAttrib |= FOREGROUND_BLUE;
	}else if (Char2 == '5'){
		wAttrib |= FOREGROUND_MAGENTA;
	}else if (Char2 == '6'){
		wAttrib |= FOREGROUND_CYAN;
	}else if (Char2 == '7'){
		wAttrib |= FOREGROUND_WHITE;
//	}else if (Char2 == '8'){ 		/* CSI for RGB color (journalctl in Debian) */
	}else if (Char2 == '9'){		/* default (ANSI v.2.53) */
		wAttrib |= FOREGROUND_DEFAULT;
	}else{
		bResult = FALSE;
	}
	Info->wAttributes = wAttrib;
	return(bResult);
}
BOOL 
AnsiRenderBackground(CONSOLE_SCREEN_BUFFER_INFO *Info, CHAR Char2)
{
	BOOL bResult = TRUE;
	WORD wAttrib = Info->wAttributes & 0xFF8F;

	if (Char2 == '0'){
		wAttrib |= BACKGROUND_BLACK;
	}else if (Char2 == '1'){
		wAttrib |= BACKGROUND_RED;
	}else if (Char2 == '2'){
		wAttrib |= BACKGROUND_GREEN;
	}else if (Char2 == '3'){
		wAttrib |= BACKGROUND_YELLOW;
	}else if (Char2 == '4'){
		wAttrib |= BACKGROUND_BLUE;
	}else if (Char2 == '5'){
		wAttrib |= BACKGROUND_MAGENTA;
	}else if (Char2 == '6'){
		wAttrib |= BACKGROUND_CYAN;
	}else if (Char2 == '7'){
		wAttrib |= BACKGROUND_WHITE;
//	}else if (Char2 == '8'){ 		/* CSI for RGB color */
	}else if (Char2 == '9'){		/* default (ANSI v.2.53) */
		wAttrib |= BACKGROUND_DEFAULT;
	}else{
		bResult = FALSE;
	}
	Info->wAttributes = wAttrib;
	return(bResult);
}
BOOL 
AnsiRenderFont(CONSOLE_SCREEN_BUFFER_INFO *Info, CHAR Char2)
{
	BOOL bResult = TRUE;

	if (!Char2 || Char2 == '0'){		/* Clear all attributes */
		Info->wAttributes = BACKGROUND_DEFAULT | FOREGROUND_DEFAULT;
	}else if (Char2 == '1'){		/* bold */
		Info->wAttributes |= FOREGROUND_INTENSITY;
	}else if (Char2 == '2'){		/* dim or secondary (journalctl on Debian) */
		bResult = TRUE;
	}else if (Char2 == '3'){		/* italic (journalctl on Debian) */
		bResult = TRUE;
	}else if (Char2 == '4'){		/* underscore (man.exe) */
		Info->wAttributes |= FOREGROUND_UNDERLINE;
	}else if (Char2 == '5'){		/* slow blink (vim.exe) */
		Info->wAttributes |= COMMON_LVB_REVERSE_VIDEO;
//	}else if (Char2 == '6'){		/* fast blink */
	}else if (Char2 == '7'){		/* invert (smso: enter standout mode) */
		Info->wAttributes |= COMMON_LVB_REVERSE_VIDEO;
	}else{
		bResult = FALSE;
	}
	return(bResult);
}
BOOL 
AnsiRenderClear(CONSOLE_SCREEN_BUFFER_INFO *Info, CHAR Char2)
{
	BOOL bResult = TRUE;

	if (Char2 == '2'){		/* bold */
		Info->wAttributes &= ~FOREGROUND_INTENSITY;
	}else if (Char2 == '4'){	/* underline */
		Info->wAttributes &= ~FOREGROUND_UNDERLINE;
	}else if (Char2 == '5'){	/* slow blink (bitchx.exe "dumb" mode) */
		Info->wAttributes &= ~COMMON_LVB_REVERSE_VIDEO;
	}else if (Char2 == '7'){	/* inversion */
		Info->wAttributes &= ~COMMON_LVB_REVERSE_VIDEO;
	}else{
		bResult = FALSE;
	}
	return(bResult);
}
BOOL 
AnsiRender(CONSOLE_SCREEN_BUFFER_INFO *Info, CHAR Char1, CHAR Char2, CHAR Char3)
{
	BOOL bResult = FALSE;

	if (Char1 == '0'){			/* raspbian login */
		bResult = AnsiRenderFont(Info, Char2);
	}else if (!Char2){
		bResult = AnsiRenderFont(Info, Char1);
	}else if (Char1 == '1'){		/* pcansi */
		bResult = AnsiRenderFont(Info, Char2);
	}else if (Char1 == '2'){		/* clear */
		bResult = AnsiRenderClear(Info, Char2);
	}else if (Char1 == '3'){		/* foreground color */
		bResult = AnsiRenderForeground(Info, Char2);
	}else if (Char1 == '4'){		/* background color */
		bResult = AnsiRenderBackground(Info, Char2);
//	}else if (Char1 == '5'){		/* steady (not blinking) */
//	}else if (Char1 == '9'){		/* future use (strike-out/cross) */
	}
	return(bResult);
}
BOOL 
AnsiSelectGraphicRendition(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, LPSTR Buffer, LONG Size)
{
	CHAR C;
	CHAR Char1 = 0;
	CHAR Char2 = 0;
	CHAR Char3 = 0;				/* 8-bit color mode (alpine.exe) */
	LPSTR psz = Buffer;
	WORD wAttribs;

	while (Size--){
		C = *Buffer++;
		if (C == '['){			/* skip CSI */
			continue;
		}else if (C == ';' || !Size){
			if (!AnsiRender(Info, Char1, Char2, Char3)){
				return(TRUE);	/* ignore */
			}
			Char1 = Char2 = Char3 = 0;
			psz = Buffer;
		}else if (!Char1){
			Char1 = C;
		}else if (!Char2){
			Char2 = C;
		}else if (!Char3){
			Char3 = C;
		}
	}
	if (Info->wAttributes & COMMON_LVB_REVERSE_VIDEO){
		wAttribs = AnsiInvertAttribs(Info);
	}else{
		wAttribs = Info->wAttributes & 0xFF;
	}
	return(SetConsoleTextAttribute(Handle, wAttribs));
}

/****************************************************/

BOOL 
AnsiCursorUp(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, WORD Count)
{
	COORD cPos = AnsiRenderCursor(Info);

	cPos.Y -= Count;
	if (cPos.Y < Info->srWindow.Top){	/* stop at top (apt [jessie]) */
		cPos.Y = Info->srWindow.Top;
	}
	Info->dwCursorPosition = cPos;
	return(SetConsoleCursorPosition(Handle, cPos));
}
BOOL 
AnsiCursorDown(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, WORD Count)
{
	COORD cPos = AnsiRenderCursor(Info);

	cPos.Y += Count;
	if (cPos.Y > Info->srWindow.Bottom){
		cPos.Y = Info->srWindow.Bottom;
	}
	Info->dwCursorPosition = cPos;
	return(SetConsoleCursorPosition(Handle, cPos));
}
BOOL 
AnsiCursorForward(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, WORD Count)
{
//	COORD cPos = Info->dwCursorPosition;
	COORD cPos = AnsiRenderCursor(Info);

	/* terminfo(5) - am: automatic right margin (mutt.exe)
	 */
	cPos.X += Count;
	if (cPos.X > Info->srWindow.Right){		/* lynx.exe */
		if (__CTTY->VEdit){
			cPos.X -= Info->srWindow.Right + 1;
			cPos.Y++;
		}else{
			cPos.X = Info->srWindow.Right;
		}
	}
	Info->dwCursorPosition = cPos;
	return(SetConsoleCursorPosition(Handle, cPos));
}
BOOL 
AnsiCursorBack(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, WORD Count)
{
//	COORD cPos = Info->dwCursorPosition;
	COORD cPos = AnsiRenderCursor(Info);

	/* terminfo(5) - bw: automatic left margin (top.exe)
	 */
	cPos.X -= Count;
	if (cPos.X < 0){				/* lynx.exe */
		if (__CTTY->VEdit){
			cPos.X += Info->srWindow.Right + 1;
			cPos.Y--;
		}else{
			cPos.X = 0;
		}
	}
	Info->dwCursorPosition = cPos;
	return(SetConsoleCursorPosition(Handle, cPos));
}
BOOL 
AnsiScrollDown(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, WORD Count)
{
	SMALL_RECT sRect = Info->srWindow;
	COORD cPos = {sRect.Left, sRect.Top};
	CHAR_INFO cInfo = {' ', Info->wAttributes & 0xFF};

	cPos.Y += Count;
	return(ScrollConsoleScreenBuffer(Handle, &sRect, &Info->srWindow, cPos, &cInfo));
}
BOOL 
AnsiScrollUp(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, WORD Count)
{
	SMALL_RECT sRect = Info->srWindow;
	COORD cPos = {sRect.Left, sRect.Top};
	CHAR_INFO cInfo = {' ', Info->wAttributes & 0xFF};

	sRect.Top += Count;
	return(ScrollConsoleScreenBuffer(Handle, &sRect, &Info->srWindow, cPos, &cInfo));
}
BOOL 
AnsiDeleteCharacter(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, WORD Count)
{
	return(AnsiScrollHorizontal(Handle, Info, Count, 0));
}
BOOL 
AnsiInsertCharacter(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, WORD Count)
{
	return(AnsiScrollHorizontal(Handle, Info, 0, Count));
}
BOOL 
AnsiInsertLine(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, WORD Count)
{
	SMALL_RECT sRect = Info->srWindow;
	COORD cPos = {0, Info->dwCursorPosition.Y};
	CHAR_INFO cInfo = {' ', Info->wAttributes & 0xFF};

	/* Clip excess lines at bottom.
	 */
	sRect.Top = cPos.Y;
	cPos.Y += Count;
	return(ScrollConsoleScreenBuffer(Handle, &sRect, &Info->srWindow, cPos, &cInfo));
}
BOOL 
AnsiDeleteLine(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, WORD Count)
{
	SMALL_RECT sRect = Info->srWindow;
	COORD cPos = {0, Info->dwCursorPosition.Y};
	CHAR_INFO cInfo = {' ', Info->wAttributes & 0xFF};

	sRect.Top = cPos.Y + Count;
	return(ScrollConsoleScreenBuffer(Handle, &sRect, &Info->srWindow, cPos, &cInfo));
}
BOOL 
AnsiEraseCharacter(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, DWORD Size)
{
//	COORD cPos = Info->dwCursorPosition;
	COORD cPos = AnsiRenderCursor(Info);
	WORD wAttribs = Info->wAttributes & 0xFF;
	DWORD dwCount;

	FillConsoleOutputAttribute(Handle, wAttribs, Size, cPos, &dwCount);
	return(FillConsoleOutputCharacter(Handle, ' ', Size, cPos, &dwCount));
}
BOOL 
AnsiCursorPosition(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, WORD Y, WORD X)
{
	COORD cPos = {X - 1, Y - 1};

	cPos.Y += Info->srWindow.Top;
	Info->dwCursorPosition = cPos;
	return(SetConsoleCursorPosition(Handle, cPos));
}
BOOL 
AnsiEraseInDisplay(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, CHAR Parm)
{
	DWORD dwCount;
	DWORD dwOffset = 0;
	SMALL_RECT sRect = Info->srWindow;
	COORD cPos = {sRect.Left, sRect.Top};
	SHORT sWidth = sRect.Right - sRect.Left + 1;
	WORD wAttribs = Info->wAttributes & 0xFF;
	DWORD dwScreen = ((sRect.Bottom - sRect.Top) + 1) * sWidth;

	if (!Parm || Parm == '0'){			/* nano.exe */
		cPos = AnsiRenderCursor(Info);
		dwOffset = ((cPos.Y - sRect.Top) + 1) * sWidth;
		dwOffset -= sWidth - cPos.X;
	}else if (Parm == '1'){
		dwOffset = (sRect.Bottom - Info->dwCursorPosition.Y) * sWidth;
		dwOffset += Info->dwCursorPosition.X;
	}else if (Parm == '2'){				/* vim.exe */
		Info->dwCursorPosition = cPos;
		SetConsoleCursorPosition(Handle, cPos);
	}else{
		return(FALSE);
	}
	FillConsoleOutputAttribute(Handle, wAttribs, dwScreen - dwOffset, cPos, &dwCount);
	return(FillConsoleOutputCharacter(Handle, ' ', dwScreen - dwOffset, cPos, &dwCount));
}
BOOL 
AnsiEraseInLine(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, CHAR Char)
{
	DWORD dwCount;
	COORD cPos = AnsiRenderCursor(Info);
	DWORD dwSize = 0;

	if (!Char || Char == '0'){
		dwSize = (Info->srWindow.Right - cPos.X) + 1;
	}else if (Char == '1'){
		cPos.X = 0;
		dwSize = Info->dwCursorPosition.X + 1;
	}else if (Char == '2'){
		cPos.X = 0;
		dwSize = Info->srWindow.Right + 1;
	}else{
		return(FALSE);
	}
	FillConsoleOutputAttribute(Handle, Info->wAttributes, dwSize, cPos, &dwCount);
	return(FillConsoleOutputCharacter(Handle, ' ', dwSize, cPos, &dwCount));
}
BOOL 
AnsiNextPage(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, WORD Count)
{
	COORD cPos = {Info->srWindow.Left, Info->srWindow.Top};
	SHORT sSize = Info->srWindow.Bottom - Info->srWindow.Top;

	cPos.Y += sSize * Count;
	Info->dwCursorPosition = cPos;
	return(SetConsoleCursorPosition(Handle, cPos));
}
BOOL 
AnsiCursorHorizontalAbsolute(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, SHORT X)
{
//	COORD cPos = {X - 1, Info->dwCursorPosition.Y};
	COORD cPos = AnsiRenderCursor(Info);

	cPos.X = X - 1;
	Info->dwCursorPosition = cPos;
	return(SetConsoleCursorPosition(Handle, cPos));
}
BOOL 
AnsiVerticalPositionAbsolute(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, SHORT Y)
{
	COORD cPos = AnsiRenderCursor(Info);

	cPos.Y -= Y;
	Info->dwCursorPosition = cPos;
	return(SetConsoleCursorPosition(Handle, cPos));
}
BOOL 
AnsiSaveCursor(CONSOLE_SCREEN_BUFFER_INFO *Info)
{
	__CTTY->Cursor = AnsiRenderCursor(Info);
	return(TRUE);
}
BOOL 
AnsiRestoreCursor(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info)
{
	BOOL bResult = TRUE;
	COORD cPos = __CTTY->Cursor;

	/* We don't have an alternate screen buffer (ALTBUF).
	 */
	if (!__CTTY->VEdit){
		cPos.Y = Info->dwCursorPosition.Y;
	}
	Info->dwCursorPosition = cPos;
	bResult = SetConsoleCursorPosition(Handle, cPos);
	return(bResult);
}
BOOL 
AnsiDeviceStatusReport(CONSOLE_SCREEN_BUFFER_INFO *Info, CHAR Parm)
{
	BOOL bResult = TRUE;
	COORD cPos = AnsiRenderCursor(Info);
	DWORD dwCount;

	if (Parm == '6'){		/* CPR - cursor position (vim.exe) */
		__Index += sprintf(&__Input[__Index], "\e[%d;%dR", (cPos.Y - Info->srWindow.Top) + 1, cPos.X + 1);
	}else{
		bResult = FALSE;
	}
	return(bResult);
}
BOOL 
AnsiDeviceAttributes(HANDLE Handle)
{
//	__Index += sprintf(&__Input[__Index], "\e[?1;2c");	/* VT100 with AVO */
//	__Index += sprintf(&__Input[__Index], "\e[?64;22c");	/* Linux */
	__Index += sprintf(&__Input[__Index], "\e[?64;1;6;15;22c");
	return(TRUE);
}
BOOL 
AnsiSetMode(HANDLE Handle, WIN_TTY *Terminal, WORD Arg)
{
	BOOL bResult = TRUE;
	DWORD dwMode;

	if (!GetConsoleMode(Handle, &dwMode)){
		bResult = FALSE;
	}else if (Arg == 4){		/* IRM (Insert/Replace mode) */
		bResult = SetConsoleMode(Handle, dwMode | ENABLE_INSERT_MODE);
	}else if (Arg == 7){		/* VEM */
		Terminal->VEdit = TRUE;
	}else{
		bResult = FALSE;
	}
	return(bResult);
}
BOOL 
AnsiResetMode(HANDLE Handle, WIN_TTY *Terminal, WORD Arg)
{
	BOOL bResult = TRUE;
	DWORD dwMode;

	if (!GetConsoleMode(Handle, &dwMode)){
		bResult = FALSE;
	}else if (Arg == 4){
		bResult = SetConsoleMode(Handle, dwMode & ~ENABLE_INSERT_MODE);
	}else if (Arg == 7){
		Terminal->VEdit = FALSE;
	}else{
		bResult = FALSE;
	}
	return(bResult);
}
BOOL 
AnsiVerticalEditingMode(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, SHORT Parm)
{
	BOOL bResult = TRUE;

	/* termcap(5) - ti: begin programs that use termcap
	 */
	if (Parm == 1){
		__CTTY->VEdit = TRUE;
	}else if (Parm == 2){
		__CTTY->VEdit = FALSE;
	}else{
		bResult = FALSE;
	}
	return(bResult);
}
BOOL 
AnsiSetTopBottomMargin(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, WORD Top, WORD Bottom)
{
	SMALL_RECT sRect = Info->srWindow;
	COORD cPos;

	if (AnsiRenderMargin(Handle, Top, Bottom, &sRect)){
		AnsiVerticalEditingMode(Handle, Info, 1);
	}else{
		AnsiVerticalEditingMode(Handle, Info, 2);
	}
	cPos.X = sRect.Left;
	cPos.Y = sRect.Top;
	Info->srWindow = sRect;
	Info->dwCursorPosition = cPos;
	return(SetConsoleCursorPosition(Handle, cPos));
}

/****************************************************/

BOOL 
AnsiControl(HANDLE Handle, CHAR C, CONSOLE_SCREEN_BUFFER_INFO *Info, SEQUENCE *Seq, DWORD Size)
{
	BOOL bResult = TRUE;

	switch (C){
		case '@':		/* ICH */
			bResult = AnsiInsertCharacter(Handle, Info, AnsiStrToInt(Seq->Args));
			break;
		case 'A':		/* CUU */
			bResult = AnsiCursorUp(Handle, Info, AnsiStrToInt(Seq->Args));
			break;
		case 'B':		/* CUD */
			bResult = AnsiCursorDown(Handle, Info, AnsiStrToInt(Seq->Args));
			break;
		case 'C':		/* CUF */
			bResult = AnsiCursorForward(Handle, Info, AnsiStrToInt(Seq->Args));
			break;
		case 'D':		/* CUB */
			bResult = AnsiCursorBack(Handle, Info, AnsiStrToInt(Seq->Args));
			break;
		case 'G':		/* CHA (nano.exe) */
			bResult = AnsiCursorHorizontalAbsolute(Handle, Info, AnsiStrToInt(Seq->Args));
			break;
		case 'H':		/* CUP */
			bResult = AnsiCursorPosition(Handle, Info, Seq->Arg1, AnsiStrToInt(Seq->Args));
			break;
		case 'J':		/* ED */
			bResult = AnsiEraseInDisplay(Handle, Info, Seq->Char1);
			break;
		case 'K':		/* EL */
			bResult = AnsiEraseInLine(Handle, Info, Seq->Char1);
			break;
		case 'L':		/* IL */
			bResult = AnsiInsertLine(Handle, Info, AnsiStrToInt(Seq->Args));
			break;
		case 'M':		/* DL */
			bResult = AnsiDeleteLine(Handle, Info, AnsiStrToInt(Seq->Args));
			break;
		case 'P':		/* DCH */
			bResult = AnsiDeleteCharacter(Handle, Info, AnsiStrToInt(Seq->Args));
			break;
		case 'S':		/* SU (Pan down) */
			bResult = AnsiScrollUp(Handle, Info, AnsiStrToInt(Seq->Args));
			break;
		case 'T':		/* SD (Pan up ) */
			bResult = AnsiScrollDown(Handle, Info, AnsiStrToInt(Seq->Args));
			break;
		case 'U':		/* NP */
			bResult = AnsiNextPage(Handle, Info, AnsiStrToInt(Seq->Args));
			break;
		case 'X':		/* ECH */
			bResult = AnsiEraseCharacter(Handle, Info, AnsiStrToInt(Seq->Args));
			break;
		case 'b':		/* VEM */
			bResult = AnsiVerticalEditingMode(Handle, Info, AnsiStrToInt(Seq->Args));
			break;
		case 'c':		/* DA */
			bResult = AnsiDeviceAttributes(Handle);
			break;
		case 'd':		/* VPA (nano.exe) */
			bResult = AnsiVerticalPositionAbsolute(Handle, Info, AnsiStrToInt(Seq->Args));
			break;
		case 'f':		/* HVP (Horizontal Vertical Position) */
			bResult = AnsiCursorPosition(Handle, Info, Seq->Arg1, AnsiStrToInt(Seq->Args));
			break;
		case 'h':		/* SM */
			bResult = AnsiSetMode(Handle, __CTTY, Seq->Arg1);
			break;
		case 'l':		/* RM */
			bResult = AnsiResetMode(Handle, __CTTY, Seq->Arg1);
			break;
		case 'm':		/* SGR */
			bResult = AnsiSelectGraphicRendition(Handle, Info, Seq->Buf, Size);
			break;
		case 'n':		/* DSR */
			bResult = AnsiDeviceStatusReport(Info, Seq->Char1);
			break;
		case 'r':		/* DECSTBM (apt) */
			bResult = AnsiSetTopBottomMargin(Handle, Info, Seq->Arg1, AnsiStrToInt(Seq->Args));
			break;
		case 's':		/* SC */
			bResult = AnsiSaveCursor(Info);
			break;
		case 'u':		/* RC */
			bResult = AnsiRestoreCursor(Handle, Info);
			break;
	}
	return(bResult);
}