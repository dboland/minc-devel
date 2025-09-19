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

/****************************************************/

VOID 
ProcDupChannels(WIN_VNODE Nodes[], WIN_VNODE Result[])
{
	DWORD dwIndex = 0;

	while (dwIndex < WIN_OPEN_MAX){
		Result->FileId = dwIndex;
		if (Nodes->Access){
			/* don't clear FD_CLOEXEC */
			vfs_F_DUPFD(Nodes, Nodes->CloseExec, Result);
		}
		dwIndex++;
		Nodes++;
		Result++;
	}
}
VOID 
ProcInheritChannels(WIN_VNODE Nodes[], HANDLE Process)
{
	DWORD dwIndex = 0;

	while (dwIndex < WIN_OPEN_MAX){
		if (Nodes->Access){
			if (Nodes->CloseExec){
				vfs_close(Nodes);
			}else if (!(Nodes->Flags & HANDLE_FLAG_INHERIT)){
				vfs_F_INHERIT(Nodes, Process);
			}
		}
		Nodes++;
		dwIndex++;
	}
}

/****************************************************/

WIN_TASK *
proc_dup(WIN_TASK *Parent, WIN_THREAD_STRUCT *Thread)
{
	WIN_TASK *ptResult = ProcCreateTask(Parent->GroupId);

	ptResult->State = WIN_SIDL;
	ptResult->Flags |= Thread->Flags;
	if (Parent->TracePoints & WIN_KTRFAC_INHERIT){
		ptResult->TraceHandle = Parent->TraceHandle;
		ptResult->TracePoints = Parent->TracePoints;
		ptResult->Flags |= WIN_PS_TRACED;
	}
	ptResult->Handle = Thread->Handle;
	ptResult->ParentId = Parent->TaskId;
	ptResult->Depth = Parent->Depth + 1;
	ptResult->GroupId = Parent->GroupId;
	ptResult->SessionId = Parent->SessionId;
	ptResult->CTTY = Parent->CTTY;
	ptResult->MountId = Parent->MountId;
	ptResult->UserSid = Parent->UserSid;
	ptResult->GroupSid = Parent->GroupSid;
	ptResult->FileMask = Parent->FileMask;
	ptResult->IsSetUGid = Parent->IsSetUGid;
	ptResult->RealUid = Parent->RealUid;
	ptResult->SavedUid = Parent->SavedUid;
	ptResult->RealGid = Parent->RealGid;
	ptResult->SavedGid = Parent->SavedGid;
	ptResult->ProcMask = Parent->ProcMask;
	ptResult->KernelTime = 0LL;
	ptResult->UserTime = 0LL;
	ptResult->IdleTime = 0LL;
	win_memcpy(ptResult->Limit, Parent->Limit, WIN_RLIM_NLIMITS * sizeof(DWORDLONG));
	win_memcpy(ptResult->AtExit, Parent->AtExit, WIN_ATEXIT_MAX * sizeof(WIN_ATEXITPROC));
	win_memcpy(ptResult->Action, Parent->Action, WIN_NSIG * sizeof(WIN_SIGACTION));
	win_memcpy(&__Strings[ptResult->TaskId], &__Strings[Parent->TaskId], sizeof(WIN_PSTRING));
	ProcDupChannels(Parent->Node, ptResult->Node);
	return(ptResult);
}
BOOL 
proc_close(WIN_TASK *Task)
{
	BOOL bResult = FALSE;

	/* In very rare (yet unknown) cases CreateProcess() is 
	 * actually *faster* than CreateThread() (building perl.exe)
	 */
	Task->ParentId = 0;		/* relinquish ownership */
	if (!Task->Handle){
		SetLastError(ERROR_INVALID_THREAD_ID);
	}else if (CloseHandle(Task->Handle)){
		bResult = TRUE;
	}else{				/* tar.exe */
		WIN_ERR("proc_close(%d): %s\n", Task->Handle, win_strerror(GetLastError()));
	}
	Task->Flags |= WIN_PS_NOZOMBIE;	/* ready to be reclaimed */
	return(bResult);
}
BOOL 
proc_execve(WIN_TASK *Task, LPSTR Command, PVOID Environ)
{
	BOOL bResult = FALSE;
	STARTUPINFO si = {0};
	HANDLE hToken = NULL;
	PROCESS_INFORMATION pi = {0};
	DWORD dwAccess = TOKEN_QUERY | TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY;
	HANDLE hThread;
	DWORD dwFlags = NORMAL_PRIORITY_CLASS;
	WIN_OBJECT_CONTROL wControl;
	SECURITY_ATTRIBUTES sa = {sizeof(sa), &wControl.Security, FALSE};

	si.cb = sizeof(STARTUPINFO);
	si.lpDesktop = "";		/* Vista */
	si.dwFlags = STARTF_PS_EXEC;
	si.dwX = Task->TaskId;
	if (!win_cap_get_proc(dwAccess, TokenPrimary, &hToken)){
		WIN_ERR("win_cap_get_proc(%s): %s\n", Command, win_strerror(GetLastError()));
	}else if (!AclCreateControl(WIN_P_IRX, WIN_P_IRX, &wControl)){
		WIN_ERR("AclCreateControl(%s): %s\n", Command, win_strerror(GetLastError()));
	}else if (CreateProcessAsUser(hToken, NULL, Command, &sa, &sa, TRUE, dwFlags, Environ, NULL, &si, &pi)){
		Task->Flags |= WIN_PS_EXEC;
		Task->ThreadId = pi.dwThreadId;
		hThread = Task->Handle;
		if (Task->ProcessId != GetCurrentProcessId()){	/* not forked (ktrace.exe) */
			Task->Handle = win_F_DISINHERIT(pi.hThread, Task->ProcessId);
		}else{
			Task->Handle = pi.hThread;
		}
		ProcInheritChannels(Task->Node, pi.hProcess);
		CloseHandle(hThread);
		CloseHandle(pi.hProcess);
		/* https://man7.org/linux/man-pages/man2/execve.2.html */
		ZeroMemory(Task->Action, WIN_NSIG * sizeof(WIN_SIGACTION));
		ZeroMemory(Task->AtExit, WIN_ATEXIT_MAX * sizeof(WIN_ATEXITPROC));
		Task->State = WIN_SRUN;
		Task->Timer = NULL;
		bResult = CloseHandle(hToken);
	}else{
		WIN_ERR("CreateProcessAsUser(%s): Handle(%d): %s\n", Command, hToken, win_strerror(GetLastError()));
	}
	proc_free(Task, Command);
	proc_free(Task, Environ);
	return(bResult);
}
BOOL 
proc_orphanize(WIN_TASK *Task)
{
	BOOL bResult = TRUE;
	WIN_TASK *pwTask = &__Tasks[WIN_PID_INIT];

	Task->ParentId = WIN_PID_INIT;
	if (!Task->Handle){		/* remote thread exited (ffmpeg.exe) */
		bResult = FALSE;
	}else if (CloseHandle(Task->Handle)){
		Task->Handle = NULL;
	}else if (ERROR_INVALID_HANDLE != GetLastError()){
		WIN_ERR("proc_orphanize(%d): %s\n", Task->Handle, win_strerror(GetLastError()));
	}
	bResult = vfs_kill_PID(pwTask->ThreadId, WM_COMMAND, CTRL_CHILD_EVENT, Task->TaskId);
	return(bResult);
}
