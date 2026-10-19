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

/************************************************************/

BOOL 
root_mount(WIN_DEVICE *Device, WIN_NAMEIDATA *Path, DWORD Flags, WIN_MODE *Mode)
{
	BOOL bResult = FALSE;
	WIN_MOUNT *pwMount = __Mounts;
	WCHAR szDrive[MAX_NAME];

	if (!SetFileAttributesW(Path->Resolved, FILE_CLASS_ROOT)){
		WIN_ERR("SetFileAttributes(%ls): %s\n", Path->Resolved, win_strerror(GetLastError()));
	}else{
		win_wcscpy(pwMount->Path, Path->Resolved);
		win_wcscpy(pwMount->TypeName, L"FFS");
		pwMount->MountId = 0;
		pwMount->FSType = Device->FSType;
		pwMount->DeviceId = Device->DeviceId;
		pwMount->DeviceType = Device->DeviceType;
		pwMount->Flags.LowPart = FILE_PERSISTENT_ACLS;
		pwMount->Flags.HighPart = WIN_MNT_ROOTFS;
		pwMount->MaxPath = WIN_PATH_MAX;
		GetSystemTimeAsFileTime(&pwMount->Time);
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
root_statvfs(WIN_MOUNT *Mount, WIN_STATVFS *Result)
{
	ZeroMemory(Result, sizeof(WIN_STATVFS));
	Result->BytesPerSector = 512;
	Result->SectorsPerCluster = 8;
	Result->ClustersTotal = 0x8000;
	Result->FreeClusters = 0x4FFF;
	win_wcscpy(Result->Path, Mount->Path);
	win_wcscpy(Result->TypeName, Mount->TypeName);
	win_wcscpy(Result->Label, Mount->Label);
	Result->DeviceId = Mount->DeviceId;
	Result->MountTime = Mount->Time;
	Result->Flags = Mount->Flags;
	Result->MaxPath = Mount->MaxPath;
	return(TRUE);
}
