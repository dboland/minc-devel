/*
 * Copyright (c) 2026 Daniel Boland <dboland@xs4all.nl>.
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
link_F_NAMEI(HANDLE Handle, DWORD Flags, WIN_NAMEIDATA *Result)
{
	BOOL bResult = TRUE;
	WCHAR szBuffer[MAX_PATH];
	DWORD dwResult;
	LPWSTR pszBase = Result->Resolved;

	if (Flags & WIN_ISSYMLINK){
		Result->Object = Handle;
	}else if (!(Flags & WIN_FOLLOW)){
		bResult = CloseHandle(Handle);
	}else if (ReadFile(Handle, szBuffer, Result->Size, &dwResult, NULL)){
		if (szBuffer[1] == ':'){
			Result->MountId = MOUNTID(szBuffer[0]);	/* nano.exe */
		}else if (szBuffer[1] == '\\'){
			Result->MountId = 0;
		}else{
			pszBase = Result->Base;
		}
		Result->R = win_wcpcpy(pszBase, szBuffer);
		Result->Base = Result->R;
		Result->Last = Result->R - 1;
		Result->Attribs = GetFileAttributesW(Result->Resolved);
		Result->FileType = WIN_VREG;
		bResult = CloseHandle(Handle);
	}else{
		bResult = FALSE;
	}
	return(bResult);
}
