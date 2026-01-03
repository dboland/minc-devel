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

VOID 
ProcInitLimits(DWORDLONG Limits[])
{
	Limits[WIN_RLIMIT_CPU] = WIN_RLIM_INFINITY;
	Limits[WIN_RLIMIT_FSIZE] = WIN_FSIZE_MAX;
	Limits[WIN_RLIMIT_DATA] = 0x00100000LL;		/* SizeOfHeapReserve? */
	Limits[WIN_RLIMIT_STACK] = WIN_STACKSIZE;
	Limits[WIN_RLIMIT_CORE] = WIN_RLIM_INFINITY;
	Limits[WIN_RLIMIT_RSS] = 0x7FFFFFFFLL;		/* ksh.exe ("memory") */
	Limits[WIN_RLIMIT_MEMLOCK] = 0x7FFFFFFFLL;
	Limits[WIN_RLIMIT_NPROC] = WIN_CHILD_MAX;
	Limits[WIN_RLIMIT_NOFILE] = WIN_OPEN_MAX;
}
VOID 
ProcInitChannels(WIN_VNODE Result[])
{
	DWORD dwIndex = 0;

	while (dwIndex < WIN_OPEN_MAX){
		Result->FileId = dwIndex++;
		Result++;
	}
}

/************************************************************/

BOOL 
proc_setugid(WIN_TASK *Task)
{
	BOOL bResult = FALSE;
	WCHAR szPath[MAX_PATH];
	WIN_VATTR wStat = {0};
	WIN_CAP_CONTROL wControl;

	if (!GetModuleFileNameW(NULL, szPath, MAX_PATH)){
		return(FALSE);
	}else if (!VfsStatFile(szPath, FILE_ATTRIBUTE_NORMAL, &wStat)){
		return(FALSE);
	}
	if (wStat.Mode.Special & WIN_S_ISUID){
		bResult = vfs_seteuid(Task, &wStat.UserSid);
	}
	if (wStat.Mode.Special & WIN_S_ISGID){
		bResult = vfs_setegid(Task, &wStat.GroupSid);
	}
	return(bResult);
}
WIN_TASK *
proc_init(WIN_SIGPROC SignalProc)
{
	STARTUPINFO si = {0};

	__SignalProc = SignalProc;
	SetUnhandledExceptionFilter(SigExceptionProc);
	SetConsoleCtrlHandler(ConControlHandler, TRUE);
	SetLastError(ERROR_SUCCESS);
	GetStartupInfo(&si);
	if (si.dwFlags & STARTF_PS_EXEC){
		__Process = &__Tasks[si.dwX];
		__Process->Flags |= WIN_PS_INEXEC;
	}else{
		__Process = ProcCreateTask(0);
		__Process->Flags |= WIN_PS_SYSTEM;
		__Process->FileMask = 0022;
		vfs_namei(GetStdHandle(STD_INPUT_HANDLE), 0, &__Process->Node[0]);
		vfs_namei(GetStdHandle(STD_OUTPUT_HANDLE), 1, &__Process->Node[1]);
		vfs_namei(GetStdHandle(STD_ERROR_HANDLE), 2, &__Process->Node[2]);
		win_getcwd(__Strings[__Process->TaskId].Path);
		win_geteuid(&__Process->UserSid);
		win_getegid(&__Process->GroupSid);
		ProcInitChannels(__Process->Node);
		ProcInitLimits(__Process->Limit);
//		SetErrorMode(SEM_FAILCRITICALERRORS);
	}
	if (proc_setugid(__Process)){
		__Process->IsSetUGid = 1;
	}
	__CTTY = TERMINAL(__Process->TerminalId);
	return(__Process);
}
DECLSPEC_NORETURN VOID 
proc_exit(DWORD ExitCode)
{
	if (__ThreadCount--){
		ExitThread(ExitCode);
	}else{
		ExitProcess(ExitCode);
	}
}
PVOID 
proc_malloc(WIN_TASK *Task, ULONG Size)
{
	ULONG ulSize = 0;
	PVOID pvResult = NULL;

	Task->Heap = GetProcessHeap();
	Size += sizeof(ULONG);
	while (ulSize < Size){
		ulSize += __Globals->PageSize;
	}
	pvResult = HeapAlloc(Task->Heap, HEAP_ZERO_MEMORY, ulSize);
	*(ULONG *)pvResult = ulSize;
	return(pvResult + sizeof(ULONG));
}
BOOL 
proc_realloc(WIN_TASK *Task, ULONG Size, PVOID Buffer, PVOID *Result)
{
	BOOL bResult = TRUE;
	PVOID pvResult = Buffer - sizeof(ULONG);
	ULONG ulSize = *(ULONG *)pvResult;

	Size += sizeof(ULONG);
	if (Size > ulSize){
		while (ulSize < Size){
			ulSize += __Globals->PageSize;
		}
		if (pvResult = HeapReAlloc(Task->Heap, HEAP_ZERO_MEMORY, pvResult, ulSize)){
			*(ULONG *)pvResult = ulSize;
			*Result = pvResult + sizeof(ULONG);
		}else{
//			WIN_ERR("HeapReAlloc(0x%x): %s\n", ulSize, win_strerror(GetLastError()));
			bResult = FALSE;
		}
	}
	return(bResult);
}
VOID 
proc_free(WIN_TASK *Task, PVOID Buffer)
{
	HeapFree(Task->Heap, 0, Buffer - sizeof(ULONG));
}
