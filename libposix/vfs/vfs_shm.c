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

LPVOID 
SHMCreate(LPCSTR Name, DWORD SizeLow)
{
	LPVOID lpvResult = NULL;
	WIN_OBJECT_CONTROL wControl;
	SECURITY_ATTRIBUTES sa = {sizeof(sa), &wControl.Security, TRUE};

	/* The initial Session handle needs to be inherited so the boot 
	 * program can replace itself with a terminal program.
	 */
	if (!AclCreateControl(WIN_S_IRW, WIN_S_IREAD, &wControl)){
		return(NULL);
	}else if (!(__Shared = CreateFileMapping(INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE, 0, SizeLow, Name))){
		WIN_ERR("CreateFileMapping(%s): %s\n", Name, win_strerror(GetLastError()));
	}else if (lpvResult = MapViewOfFile(__Shared, FILE_MAP_WRITE, 0, 0, 0)){
		ZeroMemory(lpvResult, SizeLow);
	}else{
		WIN_ERR("MapViewOfFile(%d): %s\n", __Shared, win_strerror(GetLastError()));
	}
	return(lpvResult);
}

/************************************************************/

WIN_SESSION *
vfs_shminit(LPCSTR Name, HINSTANCE Instance)
{
	WIN_SESSION *wsResult = NULL;

	if (!(__Shared = OpenFileMapping(FILE_MAP_WRITE, FALSE, Name))){
		wsResult = SHMCreate(Name, sizeof(WIN_SESSION));
		win_init(wsResult->Globals, Instance);
		pdo_init(wsResult->Devices);
	}else if (!(wsResult = MapViewOfFile(__Shared, FILE_MAP_WRITE, 0, 0, 0))){
		WIN_ERR("MapViewOfFile(%d): %s\n", __Shared, win_strerror(GetLastError()));
	}
	return(wsResult);
}
VOID 
vfs_shmexit(WIN_SESSION *Session)
{
	if (!UnmapViewOfFile(Session)){
		WIN_ERR("UnmapViewOfFile(%d): %s\n", Session, win_strerror(GetLastError()));
	}else{
		CloseHandle(__Shared);
	}
}
