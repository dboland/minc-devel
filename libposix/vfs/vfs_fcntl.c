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

#include <ddk/ntapi.h>

/****************************************************/

BOOL 
vfs_F_DUPFD(WIN_VNODE *Node, BOOL CloseExec, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;
	HANDLE hProcess = GetCurrentProcess();
	DWORD dwOptions = DUPLICATE_SAME_ACCESS;
	DWORD dwFileId = Result->FileId;

	win_memcpy(Result, Node, sizeof(WIN_VNODE));
	Result->CloseExec = CloseExec;
	Result->FileId = dwFileId;
	switch (Node->FSType){
		case FS_TYPE_WINSOCK:
		case FS_TYPE_PIPE:
			bResult = pipe_F_DUPFD(Node, hProcess, dwOptions, Result);
			break;
		case FS_TYPE_CHAR:
			bResult = char_F_DUPFD(Node, hProcess, dwOptions, Result);
			break;
		case FS_TYPE_DISK:
			bResult = disk_F_DUPFD(Node, hProcess, dwOptions, Result);
			break;
		case FS_TYPE_PDO:
			bResult = pdo_F_DUPFD(Node, hProcess, dwOptions, Result);
			break;
		case FS_TYPE_MAILSLOT:
			bResult = mail_F_DUPFD(DEVICE(Node->DeviceId), hProcess, dwOptions, Result);
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
BOOL 
vfs_F_INHERIT(WIN_VNODE *Node, HANDLE Process)
{
	BOOL bResult = FALSE;
	DWORD dwOptions = DUPLICATE_SAME_ACCESS | DUPLICATE_CLOSE_SOURCE;

	switch (Node->FSType){
		case FS_TYPE_WINSOCK:
		case FS_TYPE_PIPE:
			bResult = pipe_F_DUPFD(Node, Process, dwOptions, Node);
			break;
		case FS_TYPE_CHAR:
			bResult = char_F_DUPFD(Node, Process, dwOptions, Node);
			break;
		case FS_TYPE_DISK:
			bResult = disk_F_DUPFD(Node, Process, dwOptions, Node);
			break;
		case FS_TYPE_PDO:
			bResult = pdo_F_DUPFD(Node, Process, dwOptions, Node);
			break;
		case FS_TYPE_MAILSLOT:
			bResult = mail_F_DUPFD(DEVICE(Node->DeviceId), Process, dwOptions, Node);
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
BOOL 
vfs_F_SETFL(WIN_VNODE *Node, WIN_FLAGS *Flags)
{
	BOOL bResult = FALSE;

	switch (Node->FSType){
		case FS_TYPE_PIPE:	/* perl.exe (Configure) */
			bResult = pipe_F_SETFL(Node, Flags);
			break;
		case FS_TYPE_WINSOCK:	/* git.exe (pull) */
			bResult = ws2_F_SETFL(Node, Flags);
			break;
		default:
			Node->Access = Flags->Access;
			Node->Attribs = Flags->Attribs;
			Node->CloseExec = Flags->CloseExec;
			bResult = TRUE;
	}
	return(bResult);
}
BOOL 
vfs_F_GETFL(HANDLE Handle, ACCESS_MASK *Result)
{
	BOOL bResult = TRUE;
	PUBLIC_OBJECT_BASIC_INFORMATION pobInfo;
	ULONG ulSize = sizeof(PUBLIC_OBJECT_BASIC_INFORMATION);
	NTSTATUS ntStatus;

	ntStatus = NtQueryObject(Handle, ObjectBasicInformation, &pobInfo, ulSize, &ulSize);
	if (NT_SUCCESS(ntStatus)){
		*Result = pobInfo.GrantedAccess;
	}else{
		bResult = FALSE;
	}
	return(bResult);
}
BOOL 
vfs_F_GETPATH(WIN_VNODE *Node, SID8 *Owner, SID8 *Group, WIN_NAMEIDATA *Result)
{
	BOOL bResult = FALSE;
	NTSTATUS ntStatus;
	PUNICODE_STRING puString;
	ULONG ulSize = WIN_PATH_MAX * sizeof(WCHAR);
	LPWSTR psz;

	puString = LocalAlloc(LMEM_FIXED, ulSize);
	ntStatus = NtQueryObject(Node->Handle, ObjectNameInformation, puString, ulSize, &ulSize);
	if (!NT_SUCCESS(ntStatus)){
		WIN_ERR("NtQueryObject(%d): %s\n", Node->Handle, nt_strerror(ntStatus));
	}else if (puString->Length){
		psz = win_wcpcpy(Result->Resolved, L"\\\\.\\GLOBALROOT");
		psz = win_wcpcpy(psz, puString->Buffer);
		Result->R = psz;
		Result->S = NULL;
		Result->Base = psz;
		Result->Last = psz - 1;
		Result->Owner = Owner;
		Result->Group = Group;
		Result->FSType = Node->FSType;
		Result->Attribs = Node->Attribs;
		Result->FileType = Node->FileType;
		Result->MountId = Node->MountId;
		Result->Object = Node->Object;
		Result->Index = Node->Index;
		Result->Flags = 0;
		bResult = TRUE;
	}
	LocalFree(puString);
	return(bResult);
}
BOOL 
vfs_F_SETLK(WIN_VNODE *Node, DWORD Flags, LARGE_INTEGER *Offset, LARGE_INTEGER *Size)
{
	BOOL bResult = FALSE;

	switch (Node->FSType){
		case FS_TYPE_DISK:
			bResult = disk_F_SETLK(Node, Flags, Offset, Size);
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}

/****************************************************/

BOOL 
vfs_open(WIN_NAMEIDATA *Path, WIN_FLAGS *Flags, WIN_MODE *Mode, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;

	switch (Path->FSType){
		case FS_TYPE_PDO:
			bResult = pdo_open(Path, Flags, Result);
			break;
		case FS_TYPE_DISK:
			bResult = disk_open(Path, Flags, Mode, Result);
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
