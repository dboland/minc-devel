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
PipeSetFlags(WIN_VNODE *Node, WIN_FLAGS *Flags)
{
	BOOL bResult = FALSE;
	DWORD dwMode = PIPE_WAIT;

	if (Flags->Attribs & FILE_FLAG_OVERLAPPED){     /* perl.exe (Configure) */
		dwMode = PIPE_NOWAIT;
	}
	if (!SetNamedPipeHandleState(Node->Handle, &dwMode, NULL, NULL)){
		WIN_ERR("SetNamedPipeHandleState(%d): %s\n", Node->Handle, win_strerror(GetLastError()));
	}else{
		Node->Access = Flags->Access;
		Node->Attribs = Flags->Attribs;
		Node->CloseExec = Flags->CloseExec;
		bResult = TRUE;
	}
	return(bResult);
}

/****************************************************/

BOOL 
pipe_F_DUPFD(WIN_VNODE *Node, HANDLE Process, DWORD Options, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;
	HANDLE hResult = NULL;
	HANDLE hEvent = NULL;
	BOOL bInherit = (Node->Flags & HANDLE_FLAG_INHERIT);

	if (!DuplicateHandle(GetCurrentProcess(), Node->Handle, Process, &hResult, 0, bInherit, Options)){
		WIN_ERR("pipe_F_DUPFD(%d): %s\n", Node->Handle, win_strerror(GetLastError()));
	}else if (!DuplicateHandle(GetCurrentProcess(), Node->Event, Process, &hEvent, 0, bInherit, Options)){
		WIN_ERR("pipe_F_DUPFD(%d): %s\n", Node->Event, win_strerror(GetLastError()));
	}else{
		Result->Handle = hResult;
		Result->Event = hEvent;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
pipe_F_SETFL(WIN_VNODE *Node, WIN_FLAGS *Flags)
{
	BOOL bResult = FALSE;

	if (!(Node->Access & FILE_WRITE_DATA)){
		bResult = PipeSetFlags(Node, Flags);
	}else{
		Node->Access = Flags->Access;
		Node->Attribs = Flags->Attribs;
		Node->CloseExec = Flags->CloseExec;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
pipe_F_LOOKUP(HANDLE Handle, DWORD Flags, WIN_NAMEIDATA *Result)
{
	BOOL bResult = FALSE;
	DWORD dwResult;

	if (Flags & WIN_NEEDHANDLE){
		Result->Object = Handle;
	}else if (!(Flags & WIN_FOLLOW)){
		bResult = CloseHandle(Handle);
	}else if (ReadFile(Handle, Result->Resolved, Result->Size, &dwResult, NULL)){
		bResult = CloseHandle(Handle);
	}else{
		WIN_ERR("ReadFile(%ls): %s\n", Result->Resolved, win_strerror(GetLastError()));
	}
	return(bResult);
}
BOOL 
pipe_F_BIND(LPWSTR FileName, DWORD FileType, SECURITY_ATTRIBUTES *Attribs, LPWSTR NtName)
{
	BOOL bResult = FALSE;
	DWORD dwResult;
	WIN_INODE iNode = {TypeNameVirtual, DEV_CLASS_CPU, FileType, 
		FS_TYPE_PIPE, INAMESIZE(NtName), 0, 0};
	HANDLE hNode;

	hNode = CreateFileW(FileName, GENERIC_WRITE, FILE_SHARE_READ, 
		Attribs, CREATE_NEW, FILE_CLASS_INODE, NULL);
	if (hNode == INVALID_HANDLE_VALUE){
		return(FALSE);
	}else if (!WriteFile(hNode, &iNode, sizeof(WIN_INODE), &dwResult, NULL)){
		WIN_ERR("WriteFile(%ls): %s\n", FileName, win_strerror(GetLastError()));
	}else if (WriteFile(hNode, NtName, iNode.NameSize, &dwResult, NULL)){
		bResult = CloseHandle(hNode);
	}
	return(bResult);
}

/****************************************************/

BOOL 
pipe_open(WIN_DEVICE *Device, WIN_FLAGS *Flags, WIN_VNODE *Result)
{
	Result->DeviceType = Device->DeviceType;
	Result->DeviceId = Device->DeviceId;
	Result->Event = Device->Event;
	Result->Index = Device->Index;
	return(TRUE);
}
