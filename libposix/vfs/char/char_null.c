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
null_F_DUPFD(WIN_VNODE *Node, HANDLE Process, DWORD Options, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;
	HANDLE hResult = NULL;
	HANDLE hObject = NULL;
	BOOL bInherit = (Node->Flags & HANDLE_FLAG_INHERIT);

	if (!DuplicateHandle(GetCurrentProcess(), Node->Handle, Process, &hResult, 0, bInherit, Options)){
		WIN_ERR("null_F_DUPFD(%d): %s\n", Node->Handle, win_strerror(GetLastError()));
	}else if (!DuplicateHandle(GetCurrentProcess(), Node->Object, Process, &hObject, 0, bInherit, Options)){
		WIN_ERR("null_F_DUPFD(%d): %s\n", Node->Object, win_strerror(GetLastError()));
	}else{
		Result->Handle = hResult;
		Result->Object = hObject;
		bResult = TRUE;
	}
	return(bResult);
}

BOOL 
null_close(WIN_VNODE *Node)
{
	BOOL bResult = FALSE;

	if (!CloseHandle(Node->Handle)){
		WIN_ERR("null_close(%d): %s\n", Node->Handle, win_strerror(GetLastError()));
	}else if (!CloseHandle(Node->Object)){
		WIN_ERR("null_close(%d): %s\n", Node->Object, win_strerror(GetLastError()));
	}else{
		ZeroMemory(Node, sizeof(WIN_VNODE));
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
null_open(WIN_FLAGS *Flags, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;
	HANDLE hResult = NULL;
	SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};

	hResult = CreateFile("NUL", Flags->Access, Flags->Share, 
		&sa, Flags->Creation, Flags->Attribs, NULL);
	if (hResult != INVALID_HANDLE_VALUE){
		Result->Handle = hResult;
		Result->Event = hResult;
		Result->FSType = FS_TYPE_CHAR;
		Result->Flags = win_F_GETFD(hResult);
		Result->Access = win_F_GETFL(hResult);
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
null_fstat(WIN_VNODE *Node, WIN_VATTR *Result)
{
	BOOL bResult = FALSE;
	PSECURITY_DESCRIPTOR psd;

	if (!win_acl_get_fd(Node->Handle, &psd)){
		return(FALSE);
	}else if (GetFileInformationByHandle(Node->Object, (BY_HANDLE_FILE_INFORMATION *)Result)){
		Result->DeviceId = DEV_TYPE_ROOT;
		Result->Mode.FileType = Node->FileType;
		Result->RawDeviceId = Node->DeviceId;
		bResult = vfs_acl_stat(psd, Result);
//vfs_ktrace(L"null_fstat", STRUCT_VNODE, Node);
	}else{
		WIN_ERR("GetFileInformationByHandle(%d): %s\n", Node->Object, win_strerror(GetLastError()));
	}
	LocalFree(psd);
	return(bResult);
}
