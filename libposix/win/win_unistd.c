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

#include <shlobj.h>

const GUID CLSID_ShellLink = {0x00021401, 0, 0, {0xc0, 0, 0, 0, 0, 0, 0, 0x46}};

/**************************************************************/

BOOL 
LinkReadTarget(HANDLE Handle, LPWSTR Buffer)
{
	BOOL bResult = FALSE;
	USHORT IDListSize;
	DWORD dwResult;
	PVOID pvData;

	ReadFile(Handle, &IDListSize, sizeof(USHORT), &dwResult, NULL);
	pvData = LocalAlloc(LPTR, IDListSize);
	ReadFile(Handle, pvData, IDListSize, &dwResult, NULL);
	if (!SHGetPathFromIDListW(pvData, Buffer)){
		WIN_ERR("SHGetPathFromIDList(%d): %s\n", Handle, win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	LocalFree(pvData);
	return(bResult);
}
LPWSTR 
LinkReadNetworkInfo(COMMON_NETWORK_RELATIVE_LINK *Info, LPWSTR Buffer)
{
	BYTE *Data = (BYTE *)Info;
	LPWSTR psz;

	psz = win_mbstowcp(Buffer, &Data[Info->DeviceNameOffset], MAX_PATH);
	return(win_wcpcpy(psz, L"\\"));
}
BOOL 
LinkReadInfo(HANDLE Handle, LPWSTR Buffer)
{
	BOOL bResult = TRUE;
	LINK_INFO *Info;
	DWORD LinkInfoSize;
	BYTE *Data;
	DWORD dwResult;
	LPWSTR psz;

	ReadFile(Handle, &LinkInfoSize, sizeof(DWORD), &dwResult, NULL);
	Data = LocalAlloc(LPTR, LinkInfoSize);
	Info = (LINK_INFO *)Data;
	ReadFile(Handle, &Data[4], LinkInfoSize - 4, &dwResult, NULL);
	if (Info->LinkInfoFlags & VolumeIDAndLocalBasePath){
		win_mbstowcs(Buffer, &Data[Info->LocalBasePathOffset], MAX_PATH);
	}else if (Info->LinkInfoFlags & CommonNetworkRelativeLinkAndPathSuffix){
		psz = LinkReadNetworkInfo((PVOID)&Data[Info->CommonNetworkRelativeLinkOffset], Buffer);
		win_mbstowcs(psz, &Data[Info->CommonPathSuffixOffset], MAX_PATH - 3);
	}else{
		SetLastError(ERROR_BAD_ARGUMENTS);
		bResult = FALSE;
	}
	LocalFree(Data);
	return(bResult);
}

/**************************************************************/

BOOL 
LinkStatVolume(LPCWSTR Drive, LPWSTR Label, DWORD *Serial)
{
	BOOL bResult = FALSE;
	WCHAR szVolume[MAX_NAME];
	DWORD dwMaxPath;
	DWORD dwFlags;
	WCHAR szType[MAX_LABEL];

	if (!GetVolumeInformationW(win_volname(szVolume, Drive), Label, MAX_LABEL, 
		Serial, &dwMaxPath, &dwFlags, szType, MAX_LABEL)){
		WIN_ERR("GetVolumeInformation(%ls): %s\n", szVolume, win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}
VOID 
LinkWriteTarget(HANDLE Handle, LPCWSTR Target)
{
	USHORT IDListSize;
	USHORT TerminalID = 0;
	SFGAOF sfgaof = SFGAO_FILESYSTEM;
	LPITEMIDLIST pidList;
	UINT uiSize = 0;
	DWORD dwResult;

	/* Use type cast to get past incorrect declaration by MinGW.
	 */
	if (S_OK == SHParseDisplayName(Target, NULL, (LPITEMIDLIST)&pidList, sfgaof, &sfgaof)){
		uiSize = ILGetSize(pidList);
		IDListSize = uiSize + sizeof(USHORT);
		WriteFile(Handle, &IDListSize, sizeof(USHORT), &dwResult, NULL);
		WriteFile(Handle, pidList, uiSize, &dwResult, NULL);
		WriteFile(Handle, &TerminalID, sizeof(USHORT), &dwResult, NULL);
	}else{
		WIN_ERR("SHParseDisplayName(%ls): %s\n", Target, win_strerror(GetLastError()));
	}
}
VOLUME_ID *
LinkWriteVolumeInfo(LPCWSTR Target)
{
	VOLUME_ID *Info;
	BYTE *Data;
	WCHAR szDrive[MAX_NAME];
	DWORD dwSize = sizeof(VOLUME_ID);
	WCHAR szLabel[MAX_LABEL] = L"";
	DWORD dwSerial = 0;

	LinkStatVolume(win_drivename(szDrive, Target), szLabel, &dwSerial);
	dwSize += wcslen(szLabel) + 1;
	Data = LocalAlloc(LPTR, dwSize);
	Info = (VOLUME_ID *)Data;
	Info->VolumeIDSize = dwSize;
	Info->DriveType = GetDriveTypeW(szDrive);
	Info->DriveSerialNumber = dwSerial;
	Info->VolumeLabelOffset = sizeof(VOLUME_ID);
	win_wcstombs(&Data[Info->VolumeLabelOffset], szLabel, MAX_NAME);
	return(Info);
}
COMMON_NETWORK_RELATIVE_LINK *
LinkWriteNetworkInfo(LPCSTR NetName, LPCSTR DeviceName)
{
}
VOID 
LinkWriteInfo(HANDLE Handle, LPCWSTR Target)
{
	LINK_INFO lInfo = {0};
	VOLUME_ID *pvInfo = LinkWriteVolumeInfo(Target);
	CHAR szTarget[MAX_PATH];
	DWORD dwSize = win_wcstombs(szTarget, Target, MAX_PATH);
	DWORD dwResult;

	lInfo.LinkInfoSize = sizeof(LINK_INFO) + pvInfo->VolumeIDSize + dwSize + 1;
	lInfo.LinkInfoHeaderSize = sizeof(LINK_INFO);
	lInfo.LinkInfoFlags = VolumeIDAndLocalBasePath;
	lInfo.VolumeIDOffset = sizeof(LINK_INFO);
	lInfo.LocalBasePathOffset = lInfo.VolumeIDOffset + pvInfo->VolumeIDSize;
	WriteFile(Handle, &lInfo, sizeof(LINK_INFO), &dwResult, NULL);
	WriteFile(Handle, pvInfo, pvInfo->VolumeIDSize, &dwResult, NULL);
	WriteFile(Handle, szTarget, dwSize + 1, &dwResult, NULL);
	LocalFree(pvInfo);
}

/**************************************************************/

BOOL 
win_chroot(LPCWSTR Path)
{
	BOOL bResult = FALSE;
	WCHAR szPath[MAX_PATH];

	if (!win_realpath(Path, MAX_PATH, szPath)){
		return(FALSE);
	}else if (!SetCurrentDirectoryW(szPath)){
		WIN_ERR("SetCurrentDirectory(%ls): %s\n", szPath, win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
}
BOOL 
win_readlink(LPCWSTR Path, SHELL_LINK_HEADER *Header, LPWSTR Target)
{
	BOOL bResult = FALSE;
	DWORD dwResult;
	HANDLE hFile;

	hFile = CreateFileW(Path, FILE_READ_DATA, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE){
		WIN_ERR("CreateFile(%ls): %s\n", Path, win_strerror(GetLastError()));
	}else if (!ReadFile(hFile, Header, sizeof(SHELL_LINK_HEADER), &dwResult, NULL)){
		WIN_ERR("ReadFile(%ls): %s\n", Path, win_strerror(GetLastError()));
	}else if (!IsEqualGUID(&Header->LinkCLSID, &CLSID_ShellLink)){
		SetLastError(ERROR_BAD_ARGUMENTS);
	}else if (Header->LinkFlags & HasLinkTargetIDList){
		bResult = LinkReadTarget(hFile, Target);
	}else if (Header->LinkFlags & HasLinkInfo){
		bResult = LinkReadInfo(hFile, Target);
	}
	CloseHandle(hFile);
	return(bResult);
}
BOOL 
win_symlink(LPCWSTR Path, LPCWSTR Target)
{
	BOOL bResult = FALSE;
	HANDLE hFile;
	DWORD TerminalBlock = 0;
	WIN32_FILE_ATTRIBUTE_DATA faData;
	SHELL_LINK_HEADER slHeader = {0};
	DWORD dwResult;

	hFile = CreateFileW(Path, GENERIC_WRITE, FILE_SHARE_READ, 
		NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE){
		return(FALSE);
	}else if (GetFileAttributesExW(Target, GetFileExInfoStandard, &faData)){
		slHeader.HeaderSize = sizeof(SHELL_LINK_HEADER);
		slHeader.LinkCLSID = CLSID_ShellLink;
		slHeader.LinkFlags = HasLinkInfo | HasLinkTargetIDList;
		slHeader.FileAttributes = faData.dwFileAttributes;
		slHeader.CreationTime = faData.ftCreationTime;
		slHeader.AccessTime = faData.ftLastAccessTime;
		slHeader.WriteTime = faData.ftLastWriteTime;
		slHeader.FileSize = faData.nFileSizeLow;
		WriteFile(hFile, &slHeader, sizeof(SHELL_LINK_HEADER), &dwResult, NULL);
		LinkWriteTarget(hFile, Target);
		LinkWriteInfo(hFile, Target);
		WriteFile(hFile, &TerminalBlock, sizeof(DWORD), &dwResult, NULL);
		bResult = CloseHandle(hFile);
	}else{
		WIN_ERR("GetFileAttributesEx(%ls): %s\n", Target, win_strerror(GetLastError()));
	}
	return(bResult);
}
BOOL 
win_execve(LPSTR Command, LPCSTR Path, STARTUPINFO *Info)
{
	BOOL bResult = FALSE;
	PROCESS_INFORMATION pi = {0};

	Info->cb = sizeof(STARTUPINFO);
	Info->lpDesktop = "";			/* Vista */
	Info->dwFlags = STARTF_USESTDHANDLES;
	if (CreateProcess(NULL, Command, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, Path, Info, &pi)){
		CloseHandle(Info->hStdInput);
		CloseHandle(pi.hThread);
		WaitForSingleObject(pi.hProcess, INFINITE);
		bResult = CloseHandle(pi.hProcess);
	}
	win_free(Command);
	return(bResult);
}
