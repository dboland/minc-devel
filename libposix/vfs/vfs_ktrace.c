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

#include <libtrace.h>

/************************************************************/

BOOL 
vfs_ktrace_SET(WIN_TASK *Task, WIN_NAMEIDATA *Path, PVOID Buffer, LONG Size)
{
	BOOL bResult = FALSE;
	SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};
	HANDLE hResult;
	DWORD dwCount;
	ACCESS_MASK amAccess = GENERIC_READ + GENERIC_WRITE;
	DWORD dwShare = FILE_SHARE_READ + FILE_SHARE_WRITE;

	hResult = CreateFileW(Path->Resolved, amAccess, dwShare, 
		&sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hResult == INVALID_HANDLE_VALUE){
		return(FALSE);
	}else if (!WriteFile(hResult, Buffer, Size, &dwCount, NULL)){
		WIN_ERR("WriteFile(%d): %s\n", hResult, win_strerror(GetLastError()));
	}else{
		Task->Flags |= WIN_PS_TRACED;
		Task->TraceHandle = hResult;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
vfs_ktrace_CLEAR(WIN_TASK *Task)
{
	BOOL bResult = FALSE;

	if (!CloseHandle(Task->TraceHandle)){
		WIN_ERR("vfs_ktrace_CLEAR(CloseHandle(%d)): %s\n", Task->TraceHandle, win_strerror(GetLastError()));
	}else{
		Task->Flags &= ~WIN_PS_TRACED;
		Task->TraceHandle = NULL;
		bResult = TRUE;
	}
}

/************************************************************/

VOID 
vfs_ktrace(LPCWSTR Label, STRUCT_TYPE Type, PVOID Data)
{
	LPSTR pszBuffer = LocalAlloc(LMEM_FIXED, 4096);

	switch (Type){
		case STRUCT_VNODE:
			vfs_VNODE((WIN_VNODE *)Data, pszBuffer);
			break;
		case STRUCT_NAMEI:
			vfs_NAMEI((WIN_NAMEIDATA *)Data, pszBuffer);
			break;
		case STRUCT_CONSOLE:
			vfs_CONSOLE((DWORD *)Data, pszBuffer);
			break;
	}
	WIN_ERR("%ls%s", Label, pszBuffer);
	LocalFree(pszBuffer);
}
