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

#include <ddk/mountmgr.h>

/************************************************************/

BOOL 
DriveStatVolume(LPCWSTR Drive, WIN_MOUNT *Result)
{
	BOOL bResult = FALSE;

	if (!GetVolumeInformationW(win_volname(Result->Volume, Drive), Result->Label, MAX_LABEL, &Result->Serial, 
		&Result->MaxPath, &Result->Flags.LowPart, Result->TypeName, MAX_LABEL)){
		Result->Flags.HighPart |= WIN_MNT_DOOMED;
	}else{
		bResult = TRUE;
	}
	return(bResult);
}

/************************************************************/

BOOL 
drive_mount(WIN_DEVICE *Device, WIN_NAMEIDATA *Path, DWORD Flags, WIN_MODE *Mode)
{
	BOOL bResult = FALSE;
	LONG lMountId = MOUNTID(Path->Base[0]);
	WIN_MOUNT *pwMount = &__Mounts[lMountId];
	WCHAR szDrive[MAX_NAME];

	if (Path->Attribs & FILE_ATTRIBUTE_SYSTEM){
		SetLastError(ERROR_NOT_READY);
	}else if (!DriveStatVolume(win_drivename(szDrive, Path->Base), pwMount)){
		return(FALSE);
	}else if (!SetFileAttributesW(Path->Resolved, FILE_CLASS_MOUNT)){
		WIN_ERR("SetFileAttributes(%ls): %s\n", Path->Resolved, win_strerror(GetLastError()));
	}else{
		win_wcscpy(pwMount->Path, Path->Resolved);
		pwMount->MountId = lMountId;
		pwMount->DeviceId = Device->DeviceId;
		pwMount->DeviceType = Device->DeviceType;
		GetSystemTimeAsFileTime(&pwMount->Time);
		bResult = TRUE;
	}
	return(bResult);
}
