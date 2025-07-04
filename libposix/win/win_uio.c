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
win_writev(HANDLE Handle, const WIN_IOVEC Data[], DWORD Count, ULONG *Result)
{
	BOOL bResult = FALSE;
	DWORD dwSize = 0;
	ULONG ulSize = 0;
	PVOID pvBuffer = win_malloc(1);
	PVOID P;

	while (Count--){
		dwSize = Data->Length;
		if (!win_realloc(ulSize + dwSize, pvBuffer, &pvBuffer)){
			break;
		}else{
			P = pvBuffer + ulSize;
			win_memcpy(P, Data->Buffer, dwSize);
			ulSize += dwSize;
			Data++;
		}
	}
	if (!WriteFile(Handle, pvBuffer, ulSize, &dwSize, NULL)){
		WIN_ERR("win_writev(%d): %s\n", Handle, win_strerror(GetLastError()));
	}else{
		*Result = ulSize;
		bResult = TRUE;
	}
	win_free(pvBuffer);
	return(bResult);
}
