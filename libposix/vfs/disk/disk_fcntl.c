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
disk_F_DUPFD(WIN_VNODE *Node, HANDLE Process, DWORD Options, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;
	HANDLE hResult = NULL;
	BOOL bInherit = (Node->Flags & HANDLE_FLAG_INHERIT);

	if (Node->Handle < 0){			/* don't dup pseudo handles (-1) */
		SetLastError(ERROR_INVALID_HANDLE);
	}else if (!DuplicateHandle(GetCurrentProcess(), Node->Handle, Process, &hResult, 0, bInherit, Options)){
		WIN_ERR("disk_F_DUPFD(%d): %s\n", Node->Handle, win_strerror(GetLastError()));
	}else{
		Result->Handle = hResult;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
disk_F_NAMEI(WIN_NAMEIDATA *Path, DWORD Flags)
{
	BOOL bResult = FALSE;

	switch (Path->FileType){
		case WIN_VLNK:
			bResult = link_F_NAMEI(Path, Flags);
			break;
		case WIN_VREG:
		case WIN_VDIR:
			bResult = TRUE;
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
BOOL 
disk_F_SETLK(WIN_VNODE *Node, DWORD Flags, LARGE_INTEGER *Offset, LARGE_INTEGER *Size)
{
	BOOL bResult = FALSE;

	switch (Node->FileType){
		case WIN_VREG:
			bResult = file_F_SETLK(Node, Flags, Offset, Size);
			break;
		case WIN_VDIR:
			bResult = dir_F_SETLK(Node, Flags, Offset, Size);
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
BOOL 
disk_F_CHMOD(HANDLE Handle, DWORD MountId, WIN_MODE *Mode)
{
	BOOL bResult = FALSE;
	SECURITY_INFORMATION siType = OWNER_SECURITY_INFORMATION \
		| GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION;
	SECURITY_DESCRIPTOR sd;
	PSECURITY_DESCRIPTOR psd;

	if (!win_acl_get_fd(Handle, &psd)){
		return(FALSE);
	}else if (!vfs_acl_init(psd, MountId, Mode->Special, &sd)){
		WIN_ERR("disk_fchmod(%d): %s\n", Handle, win_strerror(GetLastError()));
	}else if (vfs_acl_chmod(psd, Mode, &sd)){
		bResult = SetKernelObjectSecurity(Handle, siType, &sd);
	}
	LocalFree(psd);
	return(bResult);
}
