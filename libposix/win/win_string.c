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

CHAR _ERR255[255];
CHAR _STR255[255];

/************************************************************/

LPSTR 
win_strerrorat(HMODULE Module, LPCSTR Label, DWORD Error)
{
	DWORD dwFlags = FORMAT_MESSAGE_FROM_SYSTEM + FORMAT_MESSAGE_IGNORE_INSERTS 
		+ FORMAT_MESSAGE_MAX_WIDTH_MASK;
	DWORD dwLanguageID = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
	LPSTR pszError = _ERR255;

	if (Module){
		dwFlags += FORMAT_MESSAGE_FROM_HMODULE;
	}
	pszError += sprintf(pszError, Label, Error);
	if (!FormatMessage(dwFlags, Module, Error, dwLanguageID, pszError, 240, NULL)){
		sprintf(pszError, "Unknown error ");
	}
	return(_ERR255);
}
LPSTR 
win_strerror(DWORD Error)
{
	return(win_strerrorat(NULL, "Error(%d): ", Error));
}
LPSTR 
nt_strerror(NTSTATUS Status)
{
	return(win_strerrorat(GetModuleHandle("NTDLL.DLL"), "Status(0x%X): ", Status));
}
LPSTR 
win_strsid(SID8 *Sid)
{
	SHORT sIndex;
	LPSTR psz = _STR255;

	psz += sprintf(psz, "S-%lu-%lu", Sid->Revision, Sid->IdentifierAuthority.Value[5]);
	for (sIndex = 0; sIndex < Sid->SubAuthorityCount; sIndex++){
		psz += sprintf(psz, "-%lu", Sid->SubAuthority[sIndex]);
	}
	return(_STR255);
}
LPSTR 
win_strobj(HANDLE Objects[], DWORD Count)
{
	LPSTR pszBuf = _STR255;
	DWORD dwIndex = 0;

	while (dwIndex < Count){
		pszBuf += sprintf(pszBuf, "[%d]", Objects[dwIndex]);
		dwIndex++;
	}
	return(_STR255);
}
DWORD 
win_strtime(SYSTEMTIME *Time, LPSTR Buffer, DWORD Size)
{
	DWORD dwResult;

	dwResult = GetTimeFormat(LOCALE_SYSTEM_DEFAULT, TIME_FORCE24HOURFORMAT, Time, NULL, Buffer, Size);
	if (!dwResult){
		WIN_ERR("GetTimeFormat(): %s\n", win_strerror(GetLastError()));
	}
	return(dwResult);
}

/************************************************************/

LPSTR 
win_strcpy(LPSTR Destination, LPCSTR Source)
{
	LPSTR szResult = Destination;

	while (*Destination = *Source++){
		Destination++;
	}
	return(szResult);
}
LPSTR 
win_strcat(LPSTR Destination, LPCSTR Source)
{
	LPSTR pszResult = Destination;

	while (*Destination){
		Destination++;
	}
	win_strcpy(Destination, Source);
	return(pszResult);
}
LPSTR 
win_strlcpy(LPSTR Destination, LPCSTR Source, LONG Length)
{
	LPSTR D = Destination;
	CHAR C;

	while (C = *Source++){
		Length--;
		if (Length < 1){
			break;
		}
		*D++ = C;
	}
	*D = 0;
	return(Destination);
}
LPSTR 
win_stpcpy(LPSTR Destination, LPCSTR Source)
{
	LPSTR D = Destination;
	CHAR C;

	while (C = *Source++){
		*D++ = C;
	}
	*D = 0;
	return(D);
}
LPSTR 
win_strlcase(LPSTR String)
{
	return(CharLower(String));
}

/************************************************************/

VOID 
win_bzero(LPVOID Buffer, DWORD Size)
{
	ZeroMemory(Buffer, Size);
}
LPCSTR 
win_index(LPCSTR String, CHAR Chr)
{
	LPCSTR pszResult = NULL;
	CHAR C;

	while (C = *String){
		if (C == Chr){
			pszResult = String;
			break;
		}
		String++;
	}
	return(pszResult);
}
VOID 
win_memcpy(LPVOID Destination, LPCVOID Source, SIZE_T Size)
{
	CopyMemory(Destination, Source, Size);
}
VOID 
win_memset(PVOID Dest, CHAR Fill, SIZE_T Size)
{
	FillMemory(Dest, Size, Fill);
}
BOOL 
win_isu8cont(UCHAR C)
{
	return((C & 0xC0) == 0x80);
}
