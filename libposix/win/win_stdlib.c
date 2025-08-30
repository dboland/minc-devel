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

/************************************************************/

PVOID 
win_malloc(ULONG Size)
{
	ULONG ulSize = 0;
	HLOCAL hLocal = NULL;

	Size += sizeof(ULONG);
	while (ulSize < Size){
		ulSize += __Globals->PageSize;
	}
	hLocal = LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, ulSize);
	*(ULONG *)hLocal = ulSize;
	return(hLocal + sizeof(ULONG));
}
BOOL 
win_realloc(ULONG Size, PVOID Buffer, PVOID *Result)
{
	BOOL bResult = TRUE;
	HLOCAL hLocal = Buffer - sizeof(ULONG);
	ULONG ulSize = *(ULONG *)hLocal;

	Size += sizeof(ULONG);
	if (Size > ulSize){
		while (ulSize < Size){
			ulSize += __Globals->PageSize;
		}
		/* I know this was allocated as LMEM_FIXED, but I would like you 
		 * to move it anyway. Trust me on this. (Raymond Chen, Stack Overflow, 2013)
		 */
		if (hLocal = LocalReAlloc(hLocal, ulSize, LMEM_MOVEABLE | LMEM_ZEROINIT)){
			*(ULONG *)hLocal = ulSize;
			*Result = hLocal + sizeof(ULONG);
		}else{
			bResult = FALSE;
		}
	}
	return(bResult);
}
VOID 
win_free(PVOID Buffer)
{
	LocalFree(Buffer - sizeof(ULONG));
}
DWORD 
win_getenv(LPCSTR Name, LPSTR Buffer, DWORD Size)
{
	DWORD dwResult;
	LPSTR B = win_stpcpy(win_stpcpy(Buffer, Name), "=");

	dwResult = B - Buffer;
	dwResult += GetEnvironmentVariable(Name, B, Size - dwResult);
	return(dwResult);
}
BOOL 
win_getcwd(LPWSTR Path)
{
	BOOL bResult = FALSE;

	if (!GetCurrentDirectoryW(MAX_PATH, Path)){
		WIN_ERR("GetCurrentDirectory(%d): %s\n", MAX_PATH, win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
win_realpath(LPCWSTR Path, DWORD Size, LPWSTR Result)
{
	BOOL bResult = FALSE;
	LPWSTR pszBase = NULL;
	WCHAR szSave = Path[0];

	/* This is the first actual hack in the system. Take advantage 
	 * of GetFullPathName() not merging slashes to resolve path name by
	 * replacing dot in the root (perl.exe).
	 */
	if (szSave == '.'){
		*(LPWSTR)Path = '\\';
	}
	if (!GetFullPathNameW(Path, Size, Result, &pszBase)){
		WIN_ERR("GetFullPathName(%ls): %s\n", Path, win_strerror(GetLastError()));
	}else{
		*Result = szSave;
		bResult = TRUE;
	}
	return(bResult);
}
