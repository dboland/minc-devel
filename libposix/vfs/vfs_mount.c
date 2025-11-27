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
vfs_getfsstat(WIN_CFDATA *Config, WIN_CFDRIVER *Driver, WIN_STATFS *Result)
{
	WIN_MOUNT wMount = {0};

	/* This function simulates the mount() process and is used 
	 * by the mkent program only.
	 */
	switch (Config->DeviceType){
		case DEV_TYPE_CDROM:
			win_wcscpy(wMount.TypeName, L"ISO9660");
			wMount.Flags.LowPart = FILE_READ_ONLY_VOLUME;
			wMount.Flags.HighPart = WIN_MNT_DOOMED;
			break;
		default:
			win_wcscpy(wMount.TypeName, L"FAT");
	}
	if (Config->DeviceType == DEV_TYPE_FLOPPY){
		wMount.Flags.HighPart |= WIN_MNT_DOOMED;
	}else if (DriveStatVolume(Config->DosPath, &wMount)){
		Result->MaxPath = wMount.MaxPath;
	}else if (ERROR_NOT_READY != GetLastError()){
//		WIN_ERR("GetVolumeInformation(%ls): %s\n", wMount.Volume, win_strerror(GetLastError()));
		return(FALSE);
	}
	if (Config->DeviceType == DEV_TYPE_REMOTE){
		wMount.Flags.HighPart |= WIN_MNT_DOOMED;
	}
	win_wcscpy(Result->Path, Config->DosPath);
	win_wcscpy(Result->TypeName, wMount.TypeName);
	Result->Flags = wMount.Flags;
	Result->DeviceId = Driver->DeviceId;
	return(TRUE);
}

/****************************************************/

BOOL 
vfs_statfs(WIN_NAMEIDATA *Path, WIN_STATFS *Result)
{
	return(drive_statfs(&__Mounts[Path->MountId], Result));
}
BOOL 
vfs_mount(WIN_VNODE *Node, WIN_NAMEIDATA *Path, DWORD Flags, WIN_MODE *Mode)
{
	BOOL bResult = FALSE;
	WIN_DEVICE *pwDevice = DEVICE(Node->DeviceId);

	if (Path->Attribs == -1){
		return(FALSE);
	}else if (Path->FileType != WIN_VDIR){
		SetLastError(ERROR_DIRECTORY);
	}else switch (Node->DeviceType){
		case DEV_TYPE_ROOT:
			bResult = pdo_mount(pwDevice, Path, Flags, Mode);
			break;
		default:
			bResult = drive_mount(pwDevice, Path, Flags, Mode);
	}
	return(bResult);
}
BOOL 
vfs_unmount(WIN_NAMEIDATA *Path)
{
	BOOL bResult = FALSE;

	if (Path->Attribs == -1){
		return(FALSE);
	}else if (Path->FileType != WIN_VDIR){
		SetLastError(ERROR_DIRECTORY);
	}else if (!(Path->Attribs & FILE_ATTRIBUTE_SYSTEM)){
		bResult = TRUE;
	}else if (!SetFileAttributesW(Path->Resolved, FILE_ATTRIBUTE_NORMAL)){
		WIN_ERR("SetFileAttributes(%ls): %s\n", Path->Resolved, win_strerror(GetLastError()));
	}else{
		ZeroMemory(&__Mounts[Path->MountId], sizeof(WIN_MOUNT));
		bResult = TRUE;
	}
	return(bResult);
}