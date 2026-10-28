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

LPCWSTR 
win_basename(LPCWSTR FileName)
{
	LPCWSTR Base = FileName;
	LPCWSTR F = FileName;
	WCHAR C;

	while (C = *F++){
		if (C == '\\'){
			Base = F;
		}
	}
	return(Base);
}
LPWSTR 
win_dirname(LPWSTR FileName)
{
	LPWSTR Base = FileName;
	LPWSTR F = FileName;
	WCHAR C;

	while (C = *F){
		if (C == '\\'){
			Base = F;
		}
		F++;
	}
	*Base = 0;
	return(FileName);
}
LPWSTR 
win_volname(LPWSTR Result, LPCWSTR FileName)
{
	LPWSTR R = Result;
	WCHAR C;

	if (!FileName){
		return(NULL);
	}
	while (C = *FileName++){
		*R++ = C;
		if (C == ':'){
			break;
		}
	}
	*R++ = '\\';
	*R = 0;
	return(Result);
}
LPWSTR 
win_drivename(LPWSTR Result, LPCWSTR FileName)
{
	LPWSTR R = Result;
	WCHAR C;

	if (FileName[0] == '.'){
		return(NULL);
	}
	while (C = *FileName++){
		if (C == ':'){
			break;
		}
		*R++ = C;
	}
	*R++ = ':';
	*R = 0;
	return(Result);
}
LPWSTR 
win_typename(LPCWSTR FileName)
{
	LPWSTR F = (LPWSTR)FileName;
	LPWSTR R = NULL;
	WCHAR C;

	while (C = *F){
		if (C == '.'){
			R = F;
		}else if (C == '\\'){
			R = NULL;
		}
		F++;
	}
	if (!R){
		R = F;
	}
	return(R);
}
BOOL 
win_flagname(DWORD Flag, LPCSTR Name, DWORD Mask, DWORD *Remain)
{
	BOOL bResult = FALSE;

	if (Mask & Flag){
		if (Name){
			WIN_ERR("[%s]", Name);
		}
		*Remain &= ~Flag;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
win_sidname(SID8 *Sid, LPSTR Buffer, DWORD Size)
{
	BOOL bResult = FALSE;
	CHAR szDomain[MAX_NAME];
	DWORD dwLength = MAX_NAME;
	SID_NAME_USE snType = 0;

	if (!LookupAccountSid(NULL, Sid, Buffer, &Size, szDomain, &dwLength, &snType)){
		WIN_ERR("LookupAccountSid(%s): %s\n", win_strsid(Sid), win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}
