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

/**************************************************************/

HMODULE 
win_dlopen(LPCWSTR FileName)
{
	HINSTANCE hResult = NULL;

	if (!FileName){
		hResult = GetModuleHandle(NULL);
	}else{
		hResult = LoadLibraryW(FileName);
	}
	return(hResult);
}
FARPROC 
win_dlsym(HMODULE Module, LPCSTR ProcName)
{
	return(GetProcAddress(Module, ProcName));
}
BOOL 
win_dlclose(HMODULE Module)
{
	return(FreeLibrary(Module));
}
BOOL 
win_dladdr(LPCVOID Address, MEMORY_BASIC_INFORMATION *Info, LPWSTR FileName)
{
	BOOL bResult = FALSE;

	if (!VirtualQuery(Address, Info, sizeof(MEMORY_BASIC_INFORMATION))){
		WIN_ERR("VirtualQuery(0x%x): %s\n", Address, win_strerror(GetLastError()));
	}else if (!GetModuleFileNameW((HMODULE)Info->AllocationBase, FileName, MAX_PATH)){
		WIN_ERR("GetModuleFileName(0x%x): %s\n", Info->AllocationBase, win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}
