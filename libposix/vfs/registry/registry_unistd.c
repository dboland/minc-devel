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

#include <winreg.h>

/****************************************************/

BOOL 
RegReadFile(HKEY Key, LPCWSTR Name, DWORD Type, LPVOID Buffer, DWORD *Size)
{
	BOOL bResult = FALSE;
	LONG lResult;

	lResult = RegQueryValueExW(Key, Name, NULL, &Type, Buffer, Size);
	if (lResult != ERROR_SUCCESS){
		SetLastError(lResult);
	}else{
		bResult = TRUE;
	}
	return(bResult);
}

/****************************************************/

BOOL 
reg_read(WIN_VNODE *Node, LPCWSTR Name, LPVOID Buffer, DWORD Size, DWORD *Result)
{
	BOOL bResult = FALSE;
	DWORD dwResult = Size;

	if (RegReadFile(Node->Key, Name, Node->Attribs, Buffer, &dwResult)){
		*Result = dwResult;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
reg_close(WIN_VNODE *Node)
{
	return(RegCloseKey(Node->Key));
}
