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

#include <sys/proc.h>

/****************************************************/

DWORD 
pid_win(pid_t pid)
{
	DWORD dwResult = 0;

	if (pid < 0 || pid >= CHILD_MAX){
		SetLastError(ERROR_BAD_ARGUMENTS);
	}else{
		dwResult = __Tasks[pid].ThreadId;
	}
	return(dwResult);
}
void 
kargv_posix(pid_t pid, int argc, char *argv[])
{
	char *buf = (char *)(argv + argc + 1);

	*argv++ = buf;
	*argv = NULL;
	pathn_posix(buf, __Strings[pid].Command, WIN_MAX_PROCTITLE);
}
u_int64_t 
ktime_posix(FILETIME *Started)
{
	DWORDLONG dwlStarted = *(DWORDLONG *)Started;
	DWORDLONG dwlRealTime;

	GetSystemTimeAsFileTime((LPFILETIME)&dwlRealTime);
	dwlRealTime -= dwlStarted;			/* 100-nanosecond intervals */
	dwlRealTime *= 0.1;				/* microseconds */
	return(dwlRealTime);
}
u_int64_t 
kticks_posix(FILETIME *Time)
{
	DWORDLONG dwlTime = *(DWORDLONG *)Time;

	dwlTime *= 0.0001;				/* milliseconds */
	return(dwlTime);
}
struct kinfo_proc *
kproc_posix(struct kinfo_proc *proc, WIN_TASK *Task)
{
	WIN_TTY *pTerminal = &__Terminals[Task->CTTY];
	wchar_t *command = win_basename(__Strings[Task->TaskId].Command);
	DWORD dwPageSize = __Globals->PageSize;
	WIN_KUSAGE wkUsage;
	struct timeval tv;
	u_int64_t rtime;

	/* see: src/sys/sys/sysctl.h */

	win_bzero(proc, sizeof(struct kinfo_proc));
	win_wcstombs(proc->p_comm, command, KI_MAXCOMLEN);
//	proc->p_flag					/* INT: P_* flags. */
	proc->p_psflags = Task->Flags;			/* INT: PS_* flags on the process. */
	proc->p_pid = Task->TaskId;
	proc->p_ppid = Task->ParentId;
	proc->p_sid = Task->SessionId;
	proc->p__pgid = Task->GroupId;
	proc->p_uid = __geteuid(Task);
	proc->p_ruid = __getuid(Task);
	proc->p_svuid = Task->SavedUid;
	proc->p_gid = __getegid(Task);
	proc->p_rgid = __getgid(Task);
	proc->p_svgid = Task->SavedGid;
	proc->p_tdev = pTerminal->DeviceId;
	proc->p_tpgid = pTerminal->GroupId;
	proc->p_vm_rssize = (Task->SetSize + dwPageSize - 1) / dwPageSize;
	proc->p_vm_tsize = (Task->TextSize + dwPageSize - 1) / dwPageSize;
	proc->p_vm_dsize = (Task->DataSize - Task->SetSize + dwPageSize - 1) / dwPageSize;
	proc->p_vm_ssize = (WIN_STACKSIZE + dwPageSize - 1) / dwPageSize;
	if (win_KERN_PROC(Task->ThreadId, &wkUsage)){

		proc->p_uvalid = 1;			/* CHAR: following p_u* members from struct user are valid */

		timeval_posix(&tv, &wkUsage.Creation);
		proc->p_ustart_sec = tv.tv_sec;
		proc->p_ustart_usec = tv.tv_usec;

		rtime = ktime_posix(&wkUsage.Creation);	/* microseconds */
		proc->p_rtime_sec = rtime * 0.000001;
		proc->p_rtime_usec = rtime - (proc->p_rtime_sec * 1000000);

		proc->p_uticks = kticks_posix(&wkUsage.User);	/* milliseconds */
		proc->p_sticks = kticks_posix(&wkUsage.Kernel) + proc->p_uticks;

		proc->p_pctcpu = (proc->p_sticks * 100000) / rtime;

	}
	proc->p_stat = Task->State;			/* CHAR: S* process status (from LWP). */
	proc->p_cpuid = Task->Processor;		/* LONG: CPU id */
	proc->p_nice = Task->Nice;
	proc->p_xstat = Task->Status >> 8;		/* U_SHORT: Exit status for wait; also stop signal. */
	proc->p_tracep = (u_int32_t)Task->TraceHandle;
	proc->p_traceflag = Task->TracePoints;
	if (proc->p_stat == SSLEEP){
		win_strlcpy(proc->p_wmesg, syscallnames[Task->Code], KI_WMESGLEN);
	}
	proc->p_tid = Task->ThreadId;
	return(proc + 1);
}

/****************************************************/

struct kinfo_proc *
proc_KERN_PROC_ALL(WIN_TASK *Task, struct kinfo_proc *buf, size_t *size)
{
	if (Task->Flags){
		if (!buf){
			*size += sizeof(struct kinfo_proc);
		}else{
			buf = kproc_posix(buf, Task);
		}
	}
	return(buf);
}
struct kinfo_proc *
proc_KERN_PROC_PGRP(WIN_TASK *Task, pid_t pgid, struct kinfo_proc *buf, size_t *size)
{
	if (Task->GroupId == pgid){
		if (!buf){
			*size += sizeof(struct kinfo_proc);
		}else{
			buf = kproc_posix(buf, Task);
		}
	}
	return(buf);
}
struct kinfo_proc *
proc_KERN_PROC_UID(WIN_TASK *Task, uid_t uid, struct kinfo_proc *buf, size_t *size)
{
	uid_t rid = rid_posix(&Task->UserSid);

	if (!uid){
		uid = ROOT_UID;
	}
	if (rid == uid){
		if (!buf){
			*size += sizeof(struct kinfo_proc);
		}else{
			buf = kproc_posix(buf, Task);
		}
	}
	return(buf);
}
struct kinfo_proc *
proc_KERN_PROC_KTHREAD(WIN_TASK *Task, int tid, struct kinfo_proc *buf, size_t *size)
{
	if (Task->ThreadId == tid){
		if (!buf){
			*size += sizeof(struct kinfo_proc);
		}else{
			buf = kproc_posix(buf, Task);
		}
	}
	return(buf);
}
