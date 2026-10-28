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
disk_close(WIN_VNODE *Node)
{
	BOOL bResult = FALSE;

	switch (Node->FileType){
		case WIN_VDIR:
			bResult = dir_close(Node);
			break;
		default:
			bResult = file_close(Node);
	}
	return(bResult);
}
BOOL 
disk_fchown(WIN_VNODE *Node, PSID NewUser, PSID NewGroup)
{
	BOOL bResult = FALSE;
	SECURITY_DESCRIPTOR sd;
	SECURITY_INFORMATION si = DACL_SECURITY_INFORMATION;
	FILETIME fTime = {0, 0};
	WIN_ACL_CONTROL wControl = {NULL, NULL, NULL, NULL};

	if (!win_acl_get_fd(Node->Handle, &wControl.Source)){
		return(FALSE);
	}else if (!win_acl_dup(wControl.Source, &sd)){
		WIN_ERR("win_acl_dup(%d): %s\n", Node->Handle, win_strerror(GetLastError()));
	}else if (vfs_acl_chown(&wControl, NewUser, NewGroup, &sd)){
		GetSystemTimeAsFileTime(&fTime);
		if (!SetUserObjectSecurity(Node->Handle, &si, &sd)){
			WIN_ERR("SetUserObjectSecurity(%d): %s\n", Node->Handle, win_strerror(GetLastError()));
		}else if (!SetFileTime(Node->Handle, &fTime, NULL, NULL)){
			WIN_ERR("SetFileTime(%lu): %s\n", fTime.dwHighDateTime, win_strerror(GetLastError()));
		}else{
			bResult = TRUE;
		}
	}
	vfs_acl_free(&wControl);
	return(bResult);
}
BOOL 
disk_chown(WIN_NAMEIDATA *Path, PSID NewUser, PSID NewGroup)
{
	BOOL bResult = FALSE;
	WIN_FLAGS wFlags = {WRITE_DAC + FILE_WRITE_ATTRIBUTES, 
		FILE_SHARE_READ, OPEN_EXISTING, 0, 0};
	WIN_VNODE vNode = {0};
	WIN_MODE wMode = {0};

	if (!disk_open(Path, &wFlags, &wMode, &vNode)){
		return(FALSE);
	}else if (disk_fchown(&vNode, NewUser, NewGroup)){
		bResult = CloseHandle(vNode.Handle);
	}
	return(bResult);
}
BOOL 
disk_rmdir(WIN_NAMEIDATA *Path)
{
	BOOL bResult = FALSE;

	if (*Path->Last == '.'){		/* GNU conftest.exe */
		SetLastError(ERROR_BAD_ARGUMENTS);
	}else if (Path->FileType != WIN_VDIR){
		SetLastError(ERROR_DIRECTORY);
	}else{
		bResult = RemoveDirectoryW(Path->Resolved);
	}
	return(bResult);
}
BOOL 
disk_unlink(WIN_NAMEIDATA *Path)
{
	BOOL bResult = FALSE;

	switch (Path->FileType){
		case WIN_VDIR:
			bResult = disk_rmdir(Path);
			break;
		case WIN_VREG:
			bResult = file_unlink(Path);
			break;
		case WIN_VLNK:
			bResult = link_unlink(Path);
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
BOOL 
disk_rename(WIN_NAMEIDATA *Path, WIN_NAMEIDATA *Result)
{
	BOOL bResult = FALSE;

	switch (Path->FileType){
		case WIN_VDIR:
			bResult = dir_rename(Path, Result);
			break;
		case WIN_VLNK:
			bResult = link_rename(Path, Result);
			break;
		case WIN_VREG:
			bResult = file_rename(Path, Result);
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
BOOL 
disk_lseek(WIN_VNODE *Node, LARGE_INTEGER *Offset, DWORD Method, LARGE_INTEGER *Result)
{
	BOOL bResult = FALSE;

	if (SetFilePointerEx(Node->Handle, *Offset, Result, Method)){
		bResult = TRUE;
//	}else{
//		WIN_ERR("SetFilePointerEx(%d): %s\n", Node->Handle, win_strerror(GetLastError()));
	}
	return(bResult);
}
BOOL 
disk_pread(WIN_VNODE *Node, LPVOID Buffer, DWORD Size, DWORDLONG Offset, DWORD *Result)
{
	BOOL bResult = FALSE;
	OVERLAPPED ovl = {0, 0, Offset & 0xFFFFFFFF, Offset >> 32, NULL};
	LARGE_INTEGER liCurrent = {0};

	if (!SetFilePointerEx(Node->Handle, liCurrent, &liCurrent, FILE_CURRENT)){
		WIN_ERR("SetFilePointerEx(%d): %s\n", Node->Handle, win_strerror(GetLastError()));
	}else if (ReadFile(Node->Handle, Buffer, Size, Result, &ovl)){
		bResult = SetFilePointerEx(Node->Handle, liCurrent, &liCurrent, FILE_BEGIN);
	}else if (ERROR_HANDLE_EOF == GetLastError()){	/* vi.exe */
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
disk_pwrite(WIN_VNODE *Node, LPCVOID Buffer, DWORD Size, DWORDLONG Offset, DWORD *Result)
{
	BOOL bResult = FALSE;
	OVERLAPPED ovl = {0, 0, Offset & 0xFFFFFFFF, Offset >> 32, NULL};
	LARGE_INTEGER liCurrent = {0, 0};

	if (!SetFilePointerEx(Node->Handle, liCurrent, &liCurrent, FILE_CURRENT)){
		WIN_ERR("SetFilePointerEx(%d): %s\n", Node->Handle, win_strerror(GetLastError()));
	}else if (WriteFile(Node->Handle, Buffer, Size, Result, &ovl)){
		bResult = SetFilePointerEx(Node->Handle, liCurrent, &liCurrent, FILE_BEGIN);
	}
	return(bResult);
}
BOOL 
disk_readlink(WIN_NAMEIDATA *Path)
{
	BOOL bResult = FALSE;
	DWORD dwResult;

	if (Path->FileType != WIN_VLNK){		/* git.exe */
		SetLastError(ERROR_BAD_ARGUMENTS);
	}else if (!ReadFile(Path->Object, Path->Resolved, Path->Size, &dwResult, NULL)){
		WIN_ERR("ReadFile(%d): %s\n", Path->Object, win_strerror(GetLastError()));
	}else{
		bResult = CloseHandle(Path->Object);
	}
	return(bResult);
}
BOOL 
disk_symlink(WIN_NAMEIDATA *Path, WIN_MODE *Mode, WIN_NAMEIDATA *Target)
{
	BOOL bResult = FALSE;
	SECURITY_DESCRIPTOR sd;
	WIN_ACL_CONTROL wControl = {Path->Owner, Path->Group, NULL, NULL};
	SECURITY_ATTRIBUTES sa = {sizeof(sa), &sd, FALSE};
	WCHAR szDirName[WIN_PATH_MAX];

	if (!win_acl_get_file(vfs_dirname(szDirName, Path->Resolved), &wControl.Source)){
		return(FALSE);
	}else if (!vfs_acl_init(&wControl, Path->MountId, Mode->Special, &sd)){
		WIN_ERR("vfs_acl_init(%ls): %s\n", szDirName, win_strerror(GetLastError()));
	}else if (vfs_acl_create(&wControl, Mode, 0, &sd)){
		bResult = disk_F_BIND(Path->Resolved, WIN_VLNK, &sa, Target->Resolved);
	}
	vfs_acl_free(&wControl);
	return(bResult);
}
