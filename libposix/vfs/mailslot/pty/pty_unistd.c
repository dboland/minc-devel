/*
 * Copyright (c) 2025 Daniel Boland <dboland@xs4all.nl>.
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
PtyReadFile(HANDLE Handle, LPSTR Buffer)
{
	BOOL bResult = FALSE;
	DWORD dwResult = 0;

	*Buffer = 0;
	if (ReadFile(Handle, Buffer, WIN_MAX_INPUT, &dwResult, NULL)){
		Buffer[dwResult] = 0;
		bResult = TRUE;
	}
	__Input = Buffer;
	return(bResult);
}

/****************************************************/

BOOL 
pty_read(WIN_TASK *Task, WIN_TTY *Terminal, LPSTR Buffer, DWORD Size, DWORD *Result)
{
	BOOL bResult = FALSE;
	CHAR C;
	DWORD dwResult = 0;
	LONG lSize = Size;
	HANDLE hInput = Terminal->Input;

	while (!bResult){
		if (lSize < 1){
			bResult = TRUE;
		}else if (C = *__Input){
			*Buffer++ = C;
			dwResult++;
			lSize--;
			__Input++;
		}else if (dwResult){
			bResult = TRUE;
		}else if (!PtyReadFile(hInput, __INPUT_BUF)){
			break;
		}else if (proc_poll(Task)){
			break;
		}
	}
	*Result = dwResult;
	return(bResult);
}
