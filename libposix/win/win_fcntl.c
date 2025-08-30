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

#include <ddk/ntapi.h>

/****************************************************/

HANDLE 
win_F_DUPFD(HANDLE Handle, DWORD Flags)
{
	HANDLE hResult = NULL;
	HANDLE hProcess = GetCurrentProcess();
	BOOL bInherit = (Flags & HANDLE_FLAG_INHERIT);

	if (!DuplicateHandle(hProcess, Handle, hProcess, &hResult, 0, bInherit, DUPLICATE_SAME_ACCESS)){
		WIN_ERR("win_F_DUPFD(%d): %s\n", Handle, win_strerror(GetLastError()));
	}
	return(hResult);
}
HANDLE 
win_F_SETFD(HANDLE Handle, DWORD Info)
{
	HANDLE hResult = NULL;

	if (!SetHandleInformation(Handle, Info, Info)){
		WIN_ERR("win_F_SETFD(%d): %s\n", Handle, win_strerror(GetLastError()));
	}else{
		hResult = Handle;
	}
	return(hResult);
}
DWORD 
win_F_GETFD(HANDLE Handle)
{
	DWORD dwResult = -1;

	if (!GetHandleInformation(Handle, &dwResult)){
		WIN_ERR("win_F_GETFD(%d): %s\n", Handle, win_strerror(GetLastError()));
	}
	return(dwResult);
}
ACCESS_MASK 
win_F_GETFL(HANDLE Handle)
{
	ACCESS_MASK amResult = -1;
	PUBLIC_OBJECT_BASIC_INFORMATION pobInfo;
	ULONG ulSize = sizeof(PUBLIC_OBJECT_BASIC_INFORMATION);
	NTSTATUS ntStatus;

	ntStatus = NtQueryObject(Handle, ObjectBasicInformation, &pobInfo, ulSize, &ulSize);
	if (!NT_SUCCESS(ntStatus)){
		WIN_ERR("win_F_GETFL(%d): %s\n", Handle, nt_strerror(ntStatus));
	}else{
		amResult = pobInfo.GrantedAccess;
	}
	return(amResult);
}
LPWSTR 
win_F_GETPATH(HANDLE Handle, LPWSTR Buffer)
{
	NTSTATUS ntStatus;
	LPWSTR pszResult = NULL;
	ULONG ulSize = MAX_PATH * sizeof(WCHAR);
	PUNICODE_STRING puString = LocalAlloc(LMEM_FIXED, ulSize);

	ntStatus = NtQueryObject(Handle, ObjectNameInformation, puString, ulSize, &ulSize);
	if (!NT_SUCCESS(ntStatus)){
		WIN_ERR("win_F_GETPATH(%d): %s\n", Handle, nt_strerror(ntStatus));
	}else if (puString->Length){
		win_wcslcpy(win_wcpcpy(Buffer, L"\\\\.\\GLOBALROOT"), puString->Buffer, ulSize);
		pszResult = Buffer;
	}else{
		*Buffer = 0;
	}
	LocalFree(puString);
	return(pszResult);
}
HANDLE 
win_F_DISINHERIT(HANDLE Handle, DWORD ProcessId)
{
	HANDLE hProcess = NULL;
	HANDLE hResult = Handle;
	DWORD dwOptions = DUPLICATE_SAME_ACCESS | DUPLICATE_CLOSE_SOURCE;

	if (!(hProcess = OpenProcess(PROCESS_DUP_HANDLE, FALSE, ProcessId))){
		WIN_ERR("OpenProcess(PROCESS_DUP_HANDLE): %s\n", win_strerror(GetLastError()));
	}else if (!DuplicateHandle(GetCurrentProcess(), Handle, hProcess, &hResult, 0, FALSE, dwOptions)){
		WIN_ERR("win_F_DISINHERIT(%d): %s\n", Handle, win_strerror(GetLastError()));
	}
	CloseHandle(hProcess);
	return(hResult);
}
HANDLE 
win_F_INHERIT(HANDLE Handle, ACCESS_MASK Access, DWORD ProcessId)
{
	HANDLE hProcess = NULL;
	HANDLE hResult = NULL;
	DWORD dwOptions = 0;

	if (!Access){
		dwOptions |= DUPLICATE_SAME_ACCESS;
	}
	if (!(hProcess = OpenProcess(PROCESS_DUP_HANDLE, FALSE, ProcessId))){
		WIN_ERR("OpenProcess(PROCESS_DUP_HANDLE): %s\n", win_strerror(GetLastError()));
	}else if (!DuplicateHandle(hProcess, Handle, GetCurrentProcess(), &hResult, Access, FALSE, dwOptions)){
		WIN_ERR("win_F_INHERIT(%d): %s\n", Handle, win_strerror(GetLastError()));
	}
	CloseHandle(hProcess);
	return(hResult);
}
