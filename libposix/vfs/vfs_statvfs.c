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

#include <ddk/ntddk.h>


/* Windows device descriptions:
 * /mnt/c/MinGW/include/ddk/ntdd*.h
 *
 * grep -r DEVICE_NAME /mnt/c/MinGW/include/ddk/
 * grep -rn DEFINE_GUID /mnt/c/MinGW/include/ddk/
 * mode.com
 * mountvol.exe
 * devcon.exe (from support.cab in Windows Driver Kit [WDK])
 * msinfo32
 *
 * Physical Device Objects (PDO) represent individual devices on a bus. 
 * It is always at the bottom of the device stack. Other drivers for the
 * device attach on top of the PDO.
 *
 * All vendor device descriptions can be found in the Windows registry:
 * HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Enum
 *
 * "Volume{}" (FS_TYPE_VOLUME) are in HKEY_LOCAL_MACHINE\SYSTEM\MountedDevices
 * which contain the FDC#, STORAGE#, IDE# types. They can be accessed
 * as directories:
 * \\.\Volume{82e82774-791f-11d7-a4cf-806d6172696f} maps to \Device\HarddiskVolume1
 *
 */

/************************************************************/

DWORD 
VfsLinkType(LPWSTR BusName)
{
	DWORD dwType = FS_TYPE_LINK;

	if (!*BusName){
//		dwType = FS_TYPE_LINK;
		wcscpy(BusName, L"LINK");
	}else if (!wcscmp(BusName, L"Volume")){
		dwType = FS_TYPE_VOLUME;
	}else if (!wcsncmp(BusName, L"NP", 2)){
		dwType = FS_TYPE_NPF;
	}
	return(dwType);
}
DWORD 
VfsBusName(LPCWSTR DosPath, LPWSTR Result)
{
	DWORD dwType = FS_TYPE_PROCESS;
	LPCWSTR P = DosPath;
	WCHAR *B = Result;
	WCHAR C;

	ZeroMemory(Result, sizeof(WCHAR) * MAX_NAME);
	while (C = *P++){
		if (C == '#'){
			dwType = FS_TYPE_PDO;
			break;
		}else if (C == ':'){
			dwType = FS_TYPE_DRIVE;
			break;
		}else if (C == '{'){
			dwType = VfsLinkType(Result);
			break;
		}
		*B++ = C;
	}
	return(dwType);
}
DWORD 
VfsClassName(LPCWSTR NtPath, LPWSTR Result)
{
	LPCWSTR P = NtPath;
	DWORD dwDepth = 0;
	WCHAR C;

	while (C = *P++){
		if (C == '\\'){
			dwDepth++;
		}else if (dwDepth == 2){
			*Result++ = C;
		}
	}
	*Result = 0;
	return(dwDepth);
}
BOOL 
VfsQueryDosDevice(LPCWSTR DosPath, LPWSTR Result)
{
	/* Kaspersky Total Security creates a random FDO device,
	 * named XXXXCtrl.
	 */
	if (QueryDosDeviceW(DosPath, Result, MAX_TEXT)){
		return(TRUE);
	}else if (ERROR_ACCESS_DENIED == GetLastError()){
		win_wcscpy(win_wcpcpy(Result, L"\\Device\\"), DosPath);
	}else{
		WIN_ERR("QueryDosDevice(%ls): %s\n", DosPath, win_strerror(GetLastError()));
	}
	return(TRUE);
}

/************************************************************/

BOOL 
vfs_setvfs(WIN_CFDATA *Config, DWORD Flags)
{
	DWORD dwSize = __Globals->PageSize;
	LPWSTR pszBuffer = win_malloc(dwSize * sizeof(WCHAR));

	ZeroMemory(Config, sizeof(WIN_CFDATA));
	while (!QueryDosDeviceW(NULL, pszBuffer, dwSize)){
		if (ERROR_INSUFFICIENT_BUFFER == GetLastError()){
			dwSize += __Globals->PageSize;
			win_realloc(dwSize * sizeof(WCHAR), pszBuffer, (PVOID *)&pszBuffer);
		}else{
			WIN_ERR("QueryDosDevice(%d): %s\n", dwSize, win_strerror(GetLastError()));
			win_free(pszBuffer);
			return(FALSE);
		}
	}
	Config->Strings = pszBuffer;
	Config->Next = pszBuffer;
	return(TRUE);
}
VOID 
vfs_endvfs(WIN_CFDATA *Config)
{
	win_free(Config->Strings);
}
BOOL 
vfs_getvfs(WIN_CFDATA *Config, DWORD Flags)
{
	BOOL bResult = FALSE;
	LPCWSTR pszNext = Config->Next;

	if (!*pszNext){
		SetLastError(ERROR_NO_MORE_ITEMS);
	}else if (VfsQueryDosDevice(pszNext, Config->NtPath)){
		Config->FSType = VfsBusName(pszNext, Config->BusName);
		Config->Depth = VfsClassName(Config->NtPath, Config->ClassName);
		win_wcsucase(Config->BusName);
		Config->NtName = win_basename(Config->NtPath);
		Config->DosPath = pszNext;
		Config->Next += wcslen(pszNext) + 1;
		bResult = TRUE;
	}
	return(bResult);
}
