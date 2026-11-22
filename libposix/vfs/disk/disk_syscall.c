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
DiskCreateFile(WIN_NAMEIDATA *Path, WIN_FLAGS *Flags, WIN_MODE *Mode, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;
	SECURITY_DESCRIPTOR sd;
	WCHAR szDirName[WIN_PATH_MAX] = L"";
	HANDLE hResult;
	WIN_ACL_CONTROL wControl;
	SECURITY_ATTRIBUTES sa = {sizeof(sa), &sd, TRUE};

	Flags->Access |= WRITE_DAC | WRITE_OWNER;		/* disk_fchown() */
	if (!win_acl_get_file(vfs_dirname(szDirName, Path->Resolved), &wControl.Source)){
		return(FALSE);
	}else if (!vfs_acl_init(wControl.Source, Path->MountId, Mode->Special, &sd)){
		WIN_ERR("vfs_acl_init(%s): %s\n", szDirName, win_strerror(GetLastError()));
	}else if (vfs_acl_create(&wControl, Mode, 0, &sd)){
		hResult = CreateFileW(Path->Resolved, Flags->Access, Flags->Share, 
			&sa, Flags->Creation, Flags->Attribs, NULL);
		if (hResult != INVALID_HANDLE_VALUE){
			Result->Handle = hResult;
			Result->MountId = Path->MountId;
			Result->FSType = Path->FSType;
			Result->FileType = Path->FileType;
			Result->Attribs = Flags->Attribs;
			Result->DeviceId = Path->DeviceId;
			Result->CloseExec = Flags->CloseExec;
			Result->Access = win_F_GETFL(hResult);
			Result->Flags = win_F_GETFD(hResult);
			bResult = TRUE;
//		}else{
//			WIN_ERR("CreateFile(%ls): %s\n", Path->Resolved, win_strerror(GetLastError()));
		}
	}
	vfs_acl_free(&wControl);
	return(bResult);
}
BOOL 
DiskOpenFile(WIN_NAMEIDATA *Path, WIN_FLAGS *Flags, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;
	HANDLE hResult;
	SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};

	hResult = CreateFileW(Path->Resolved, Flags->Access, Flags->Share, 
		&sa, Flags->Creation, Flags->Attribs, NULL);
	if (hResult != INVALID_HANDLE_VALUE){
		Result->Handle = hResult;
		Result->MountId = Path->MountId;
		Result->FSType = Path->FSType;
		Result->FileType = Path->FileType;
		Result->Attribs = Flags->Attribs;
		Result->DeviceId = Path->DeviceId;
		Result->CloseExec = Flags->CloseExec;
		Result->Access = win_F_GETFL(hResult);
		Result->Flags = win_F_GETFD(hResult);
		bResult = TRUE;
//	}else{
//		WIN_ERR("CreateFile(%ls): %s\n", Path->Resolved, win_strerror(GetLastError()));
	}
	return(bResult);
}
BOOL 
DiskStatHandle(HANDLE Handle, WIN_VATTR *Result)
{
	BOOL bResult = FALSE;
	PSECURITY_DESCRIPTOR psd;

	if (!win_acl_get_fd(Handle, &psd)){
		return(FALSE);
	}else if (GetFileInformationByHandle(Handle, (BY_HANDLE_FILE_INFORMATION *)Result)){
		bResult = vfs_acl_stat(psd, Result);
	}else if (ERROR_INVALID_FUNCTION == GetLastError()){	/* Windows devices: COM, AUX (rm.exe) */
		SetLastError(ERROR_BAD_FILE_TYPE);
	}else{
		WIN_ERR("GetFileInformationByHandle(%d): %s\n", Handle, win_strerror(GetLastError()));
	}
	LocalFree(psd);
	return(bResult);
}
BOOL 
DiskStatFile(LPCWSTR FileName, DWORD Attribs, WIN_VATTR *Result)
{
	BOOL bResult = FALSE;
	HANDLE hFile;

	hFile = CreateFileW(FileName, READ_CONTROL, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, Attribs, NULL);
	if (hFile == INVALID_HANDLE_VALUE){
		return(FALSE);
	}else if (DiskStatHandle(hFile, Result)){
		bResult = CloseHandle(hFile);
	}
	return(bResult);
}
