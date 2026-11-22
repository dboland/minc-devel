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
		case WIN_VREG:
		case WIN_VLNK:
			bResult = file_fstat(Node, Result);
			break;
		case WIN_VDIR:
			bResult = dir_fstat(Node, Result);
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
		case WIN_VREG:
			bResult = file_stat(Path, Result);
			break;
		case WIN_VDIR:
			bResult = dir_stat(Path, Result);
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
disk_fchmod(WIN_VNODE *Node, WIN_MODE *Mode)
{
	BOOL bResult = FALSE;

	if (disk_F_CHMOD(Node->Handle, Node->MountId, Mode)){
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
disk_chmod(WIN_NAMEIDATA *Path, WIN_MODE *Mode)
{
	BOOL bResult = FALSE;
	HANDLE hFile;
	ACCESS_MASK amAccess = READ_CONTROL | WRITE_DAC | WRITE_OWNER;

	if (Path->FileType == WIN_VDIR){
		Path->Attribs |= FILE_FLAG_BACKUP_SEMANTICS;
	}
	hFile = CreateFileW(Path->Resolved, amAccess, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, Path->Attribs, NULL);
	if (hFile == INVALID_HANDLE_VALUE){
		return(FALSE);
	}else if (disk_F_CHMOD(hFile, Path->MountId, Mode)){
		bResult = CloseHandle(hFile);
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
