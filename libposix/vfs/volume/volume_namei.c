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

#include <ddk/ntddstor.h>

/****************************************************/

BOOL 
VolLookupPartition(HANDLE Handle, LPSTR Result)
{
	BOOL bResult = FALSE;
	STORAGE_DEVICE_NUMBER Info;
	DWORD dwSize = sizeof(STORAGE_DEVICE_NUMBER);

	if (DeviceIoControl(Handle, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0, &Info, dwSize, &dwSize, NULL)){
		sprintf(Result, "DeviceType(%d) DeviceNumber(%d) PartitionNumber(%d)", 
			Info.DeviceType, Info.DeviceNumber, Info.PartitionNumber);
		bResult = TRUE;
	}
	return(bResult);
}

/****************************************************/

BOOL 
vol_lookup(LPCWSTR Name, LPSTR Result)
{
	BOOL bResult = FALSE;
	WIN_VNODE vNode = {0};
	HANDLE hResult;
	WCHAR szPath[MAX_PATH] = L"\\\\.\\";

	hResult = CreateFileW(win_wcscat(szPath, Name), READ_CONTROL, FILE_SHARE_READ, NULL, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hResult == INVALID_HANDLE_VALUE){
		WIN_ERR("CreateFile(%ls): %s\n", szPath, win_strerror(GetLastError()));
	}else if (VolLookupPartition(hResult, Result)){
		bResult = CloseHandle(hResult);
	}
	return(bResult);
}
