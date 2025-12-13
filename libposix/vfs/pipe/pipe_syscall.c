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

#include <mswsock.h>

/****************************************************/

BOOL 
PipeCreateFile(LPCWSTR Name, DWORD Attribs, HANDLE Event, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;
	ACCESS_MASK aMask = GENERIC_READ + GENERIC_WRITE;
	DWORD dwOpenMode = (Attribs & 0xFFFF0000) | PIPE_ACCESS_DUPLEX;
	DWORD dwPipeMode = (Attribs & 0x0000FFFF) | PIPE_WAIT;
	WCHAR szPath[MAX_PATH] = L"\\\\.\\PIPE\\";
	DWORD dwMax = PIPE_UNLIMITED_INSTANCES;
	SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, FALSE};
	HANDLE hResult;

	hResult = CreateNamedPipeW(win_wcscat(szPath, Name), dwOpenMode, dwPipeMode, 
		dwMax, WIN_PIPE_BUF, WIN_PIPE_BUF, NMPWAIT_USE_DEFAULT_WAIT, &sa);
	if (hResult != INVALID_HANDLE_VALUE){
		Result->Handle = hResult;
		Result->Event = Event;
		Result->FSType = FS_TYPE_PIPE;
		Result->Attribs = Attribs & 0xFFFF0000;
		Result->Access = win_F_GETFL(hResult);
		Result->Flags = win_F_GETFD(hResult);
		bResult = TRUE;
//	}else{
//		WIN_ERR("CreateNamedPipe(%ls): %s\n", szPath, win_strerror(GetLastError()));
	}
	return(bResult);
}
BOOL 
PipeOpenFile(LPCWSTR Name, HANDLE Event, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;
	ACCESS_MASK aMask = GENERIC_WRITE + GENERIC_READ;
	DWORD dwShare = FILE_SHARE_WRITE + FILE_SHARE_READ;
	WCHAR szPath[MAX_PATH] = L"\\\\.\\PIPE\\";
	DWORD dwAttribs = FILE_ATTRIBUTE_NORMAL;
	SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, FALSE};
	HANDLE hResult;

	hResult = CreateFileW(win_wcscat(szPath, Name), aMask, dwShare, NULL, 
		OPEN_EXISTING, dwAttribs, &sa);
	if (hResult != INVALID_HANDLE_VALUE){
		Result->Handle = hResult;
		Result->Event = Event;
		Result->FSType = FS_TYPE_PIPE;
		Result->Attribs = dwAttribs;
		Result->Access = win_F_GETFL(hResult);
		Result->Flags = win_F_GETFD(hResult);
		bResult = TRUE;
//	}else{
//		WIN_ERR("PipeOpenFile(%ls): %s\n", szPath, win_strerror(GetLastError()));
	}
	return(bResult);
}
PVOID 
PipeAllocData(WSAMSG *Message, DWORD *Size, DWORD *Result)
{
	DWORD dwSize = 0;
	DWORD dwIndex = 0;
	DWORD dwCount = Message->dwBufferCount;
	LPWSABUF pwsData = Message->lpBuffers;

	while (dwIndex < dwCount){
		dwSize += pwsData->len;
		pwsData++;
		dwIndex++;
	}
	*Result = dwSize;
	dwSize += Message->Control.len;
	*Size = dwSize;
	return(win_malloc(dwSize));
}
PVOID 
PipePushDatagram(PVOID Data, LPWSABUF Buffers, DWORD Count)
{
	DWORD dwIndex = 0;

	while (dwIndex < Count){
		win_memcpy(Data, Buffers->buf, Buffers->len);
		Data += Buffers->len;
		Buffers++;
		dwIndex++;
	}
	return(Data);
}
PVOID 
PipePopDatagram(PVOID Data, LPWSABUF Buffers, DWORD Count)
{
	DWORD dwIndex = 0;

	while (dwIndex < Count){
		win_memcpy(Buffers->buf, Data, Buffers->len);
		Data += Buffers->len;
		Buffers++;
		dwIndex++;
	}
	return(Data);
}
