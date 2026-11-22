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

	*Result = *Node;
	Result->CloseExec = CloseExec;
	switch (Node->FSType){
		case FS_TYPE_WINSOCK:
		case FS_TYPE_PIPE:
			bResult = pipe_F_DUPFD(Node, hProcess, dwOptions, Result);
			break;
		case FS_TYPE_CHAR:
			bResult = char_F_DUPFD(Node, hProcess, dwOptions, Result);
			break;
		case FS_TYPE_PDO:
			bResult = pdo_F_DUPFD(Node, hProcess, dwOptions, Result);
			break;
		default:
			bResult = disk_F_DUPFD(Node, hProcess, dwOptions, Result);
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
		case FS_TYPE_PDO:
			bResult = pdo_F_DUPFD(Node, Process, dwOptions, Node);
			break;
		default:
			bResult = disk_F_DUPFD(Node, Process, dwOptions, Node);
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
vfs_F_GETPATH(WIN_VNODE *Node, WIN_NAMEIDATA *Result)
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
HANDLE 
vfs_F_OSFHANDLE(WIN_VNODE Nodes[], DWORD Index)
{
	HANDLE hResult = NULL;
	WIN_VNODE *pwNode = &Nodes[Index];

	switch (pwNode->FSType){
		case FS_TYPE_CHAR:
			hResult = char_F_OSFHANDLE(pwNode, Index);
			break;
		case FS_TYPE_MAILSLOT:
			hResult = mail_F_OSFHANDLE(pwNode, Index);
			break;
		case FS_TYPE_DISK:
			hResult = pwNode->Handle;
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(hResult);
}
BOOL 
vfs_F_NAMEI(WIN_NAMEIDATA *Path, DWORD Flags, HANDLE *Result)
{
	BOOL bResult = FALSE;
	DWORD dwSize;
	HANDLE hResult;
	SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};
	WIN_INODE iNode = {0};
	ACCESS_MASK amAccess = GENERIC_READ | READ_CONTROL;

	if (Flags & WIN_LOCKLEAF){
		amAccess |= WRITE_DAC | WRITE_OWNER;
	}
	hResult = CreateFileW(Path->Resolved, amAccess, FILE_SHARE_READ, 
		&sa, OPEN_EXISTING, FILE_CLASS_INODE, NULL);
	if (hResult == INVALID_HANDLE_VALUE){
		return(FALSE);
	}else if (!ReadFile(hResult, &iNode, sizeof(WIN_INODE), &dwSize, NULL)){
		WIN_ERR("ReadFile(%ls): %s\n", Path->Resolved, win_strerror(GetLastError()));
	}else if (iNode.Magic == TypeNameVirtual){
		Path->DeviceId = iNode.DeviceId;
		Path->FileType = iNode.FileType;
		Path->FSType = iNode.FSType;
		Path->Size = iNode.NameSize;
		Path->Object = hResult;
		bResult = TRUE;
	}else if (CloseHandle(hResult)){
		SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
