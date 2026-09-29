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
disk_namei(WIN_NAMEIDATA *Path, DWORD Flags)
{
	BOOL bResult = FALSE;
	HANDLE hResult = NULL;

	if (!VfsStatNode(Path, Flags, &hResult)){	/* Windows IFS node (.cat files) */
		Path->FSType = FS_TYPE_DISK;
		Path->FileType = WIN_VREG;
		bResult = TRUE;
	}else switch (Path->FSType){
		case FS_TYPE_DISK:
			bResult = disk_F_NAMEI(hResult, Flags, Path);
			break;
		case FS_TYPE_PIPE:
			bResult = pipe_F_NAMEI(hResult, Flags, Path);
			break;
		case FS_TYPE_PDO:
			bResult = pdo_F_NAMEI(hResult, Flags, Path);
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
BOOL 
disk_lookup(HANDLE Handle, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;
	BY_HANDLE_FILE_INFORMATION fInfo;

	Result->Handle = Handle;
	Result->FSType = FS_TYPE_DISK;
	Result->DeviceType = DEV_TYPE_ROOT;
	Result->DeviceId = DEV_TYPE_ROOT;
	if (GetFileInformationByHandle(Handle, &fInfo)){
		if (fInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
			Result->FileType = WIN_VDIR;
		}else{
			Result->FileType = WIN_VREG;
		}
		Result->Attribs = fInfo.dwFileAttributes;
		Result->Flags = win_F_GETFD(Handle);
		Result->Access = win_F_GETFL(Handle);
		bResult = TRUE;
	}else{
		WIN_ERR("GetFileInformationByHandle(%d): %s\n", Handle, win_strerror(GetLastError()));
	}
	return(bResult);
}
