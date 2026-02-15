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
disk_fstat(WIN_VNODE *Node, WIN_VATTR *Result)
{
	BOOL bResult = FALSE;

	switch (Node->FileType){
		case WIN_VDIR:
			bResult = dir_fstat(Node, Result);
			break;
		case WIN_VREG:
		case WIN_VLNK:
			bResult = file_fstat(Node, Result);
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
BOOL 
disk_stat(WIN_NAMEIDATA *Path, WIN_VATTR *Result)
{
	BOOL bResult = FALSE;

	switch (Path->FileType){
		case WIN_VDIR:
			bResult = dir_stat(Path, Result);
			break;
		case WIN_VREG:
			bResult = file_stat(Path, Result);
			break;
		case WIN_VLNK:
			bResult = link_stat(Path, Result);
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
BOOL 
disk_fchmod(WIN_VNODE *Node, SID8 *Owner, SID8 *Group, WIN_MODE *Mode)
{
	BOOL bResult = FALSE;
	SECURITY_INFORMATION siType = OWNER_SECURITY_INFORMATION + GROUP_SECURITY_INFORMATION + DACL_SECURITY_INFORMATION;
	SECURITY_DESCRIPTOR sd;
	DWORD dwSize = 0;
	WIN_ACL_CONTROL wControl = {Owner, Group, NULL, NULL};

	if (!win_acl_get_fd(Node->Handle, &wControl.Source)){
		return(FALSE);
	}else if (!vfs_acl_init(&wControl, Node->MountId, Mode->Special, &sd)){
		WIN_ERR("disk_fchmod(%d): %s\n", Node->Handle, win_strerror(GetLastError()));
	}else if (vfs_acl_chmod(&wControl, Mode, &sd)){
		bResult = SetUserObjectSecurity(Node->Handle, &siType, &sd);
	}
	vfs_acl_free(&wControl);
	return(bResult);
}
BOOL 
disk_chmod(WIN_NAMEIDATA *Path, WIN_MODE *Mode)
{
	BOOL bResult = FALSE;
	WIN_FLAGS wFlags = {READ_CONTROL + WRITE_DAC + WRITE_OWNER, 
		FILE_SHARE_READ, OPEN_EXISTING, 0};
	WIN_VNODE vNode = {0};
	WIN_MODE wMode = {0};

	if (!disk_open(Path, &wFlags, &wMode, &vNode)){
		return(FALSE);
	}else if (disk_fchmod(&vNode, Path->Owner, Path->Group, Mode)){
		bResult = CloseHandle(vNode.Handle);
	}
	return(bResult);
}
/* BOOL 
disk_fchflags(WIN_VNODE *Node, ULONG Attributes)
{
	BOOL bResult = FALSE;
	NTSTATUS ntStatus;
	IO_STATUS_BLOCK ioStatus;
	FILE_ATTRIBUTE_TAG_INFORMATION fatInfo = {Attributes, 0};

	ntStatus = NtSetInformationFile(Node->Handle, &ioStatus, &fatInfo, sizeof(FILE_ATTRIBUTE_TAG_INFORMATION), FileAttributeTagInformation);
	if (!NT_SUCCESS(ntStatus)){
		WIN_ERR("NtSetInformationFile(%d): %s\n", Node->Handle, nt_strerror(ntStatus));
	}else{
		bResult = TRUE;
	}
	return(bResult);
} */
BOOL 
disk_chflags(WIN_NAMEIDATA *Path, DWORD Attributes)
{
	return(SetFileAttributesW(Path->Resolved, Attributes));
}
