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

WIN_THREAD_STRUCT *
proc_fork_enter(PVOID Data[], DWORD Flags)
{
	WIN_THREAD_STRUCT *ptsResult;

	ptsResult = LocalAlloc(LMEM_FIXED + LMEM_ZEROINIT, sizeof(WIN_THREAD_STRUCT));
	ptsResult->TaskId = CURRENT;
	ptsResult->ThreadId = GetCurrentThreadId();
	ptsResult->Flags = Flags;
	OpenThreadToken(GetCurrentThread(), TOKEN_ALL_ACCESS, TRUE, &ptsResult->Token);
	return(ptsResult);
}
DWORD 
proc_fork(LPTHREAD_START_ROUTINE StartAddress, WIN_THREAD_STRUCT *Thread)
{
	MSG msg = {0};
	DWORD dwThreadId = 0;

	__ThreadCount++;
	Thread->Handle = CreateThread(NULL, WIN_STACKSIZE, StartAddress, Thread, 0, &dwThreadId);
	/* wait for child to send TaskId */
	GetMessage(&msg, NULL, WM_USER, WM_USER);
	return(msg.wParam);
}
VOID 
proc_fork_leave(WIN_THREAD_STRUCT *Thread)
{
	if (Thread->Token){
		SetThreadToken(NULL, Thread->Token);
		CloseHandle(Thread->Token);
	}
	if (!(Thread->Flags & WIN_PS_PPWAIT)){
		PostThreadMessage(Thread->ThreadId, WM_USER, Thread->Result, 0);
	}
	LocalFree(Thread);
}
