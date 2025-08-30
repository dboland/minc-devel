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

#include <sys/ktrace.h>

#define KTRARG(arg)	arg ? *(register_t *)arg : 0

/****************************************************/

void 
ttime_posix(struct timespec *ts, DWORDLONG *Time)
{
	DWORDLONG dwlTime = *Time;

	ts->tv_sec = (time_t)(dwlTime * 0.000000001);	/* seconds (gcc needs cast!) */
	ts->tv_nsec = dwlTime - (ts->tv_sec * 1000000000);
}

/****************************************************/

void 
ktrace_SYSCALL(WIN_TASK *Task, register_t code, size_t argsize, register_t args[])
{
	struct ktr_header header = {0};
	struct ktr_syscall data;
	WIN_IOVEC iovData[4] = {
		{(PVOID)&header, sizeof(struct ktr_header)}, 
		{(PVOID)&data, sizeof(struct ktr_syscall)},
		{(PVOID)args, argsize},
		{NULL, 0}
	};
	DWORD dwCount = 3;
	ULONG ulResult;
	register_t buf[4] = {0};

	if (code == SYS___sysctl){
		iovData[3].Buffer = (void *)args[0];
		iovData[3].Length = args[1] * sizeof(register_t);
		dwCount++;
	}else if (code == SYS_sigprocmask){
		iovData[2].Buffer = (PVOID)buf;
		buf[0] = args[0];
		buf[1] = KTRARG(args[1]);
		buf[2] = KTRARG(args[2]);
	}else if (code == SYS_sigsuspend){
		iovData[2].Buffer = (PVOID)buf;
		buf[0] = KTRARG(args[0]);
	}
	header.ktr_type = KTR_SYSCALL;
	header.ktr_pid = Task->TaskId;
	header.ktr_tid = Task->ThreadId;
	ttime_posix(&header.ktr_time, &Task->ClockTime);
	win_strlcpy(header.ktr_comm, __PROGNAME, MAXCOMLEN);
	header.ktr_len = sizeof(struct ktr_syscall) + argsize + iovData[3].Length;
	data.ktr_code = code;
	data.ktr_argsize = argsize;
	win_writev(Task->TraceHandle, iovData, dwCount, &ulResult);
}
void 
ktrace_SYSRET(WIN_TASK *Task, register_t code, int result)
{
	struct ktr_header header = {0};
	struct ktr_sysret data = {0};
	WIN_IOVEC iovData[2] = {
		{(PVOID)&header, sizeof(struct ktr_header)},
		{(PVOID)&data, sizeof(struct ktr_sysret)}
	};
	ULONG ulResult;

	header.ktr_type = KTR_SYSRET;
	header.ktr_pid = Task->TaskId;
	header.ktr_tid = Task->ThreadId;
	ttime_posix(&header.ktr_time, &Task->ClockTime);
	win_strlcpy(header.ktr_comm, __PROGNAME, MAXCOMLEN);
	header.ktr_len = sizeof(struct ktr_sysret);
	data.ktr_code = code;
//	data.ktr_eosys = errno_posix(GetLastError());
	data.ktr_retval = result;
	/* kdump.exe always prints -1 when ktr_error */
	if (result < 0){
		data.ktr_error = Task->Error;
	}
	win_writev(Task->TraceHandle, iovData, 2, &ulResult);
}
void 
ktrace_NAMEI(WIN_TASK *Task, const char *path, size_t size)
{
	struct ktr_header header = {0};
	WIN_IOVEC iovData[2] = {
		{(PVOID)&header, sizeof(struct ktr_header)},
		{(PVOID)path, size}
	};
	ULONG ulResult;

	header.ktr_type = KTR_NAMEI;
	header.ktr_pid = Task->TaskId;
	header.ktr_tid = Task->ThreadId;
	ttime_posix(&header.ktr_time, &Task->ClockTime);
	win_strlcpy(header.ktr_comm, __PROGNAME, MAXCOMLEN);
	header.ktr_len = size;
	win_writev(Task->TraceHandle, iovData, 2, &ulResult);
}
void 
ktrace_PSIG(WIN_TASK *Task, int signo, sig_t action, siginfo_t *info)
{
	struct ktr_header header = {0};
	struct ktr_psig data;
	WIN_IOVEC iovData[2] = {
		{(PVOID)&header, sizeof(struct ktr_header)},
		{(PVOID)&data, sizeof(struct ktr_psig)}
	};
	ULONG ulResult;
	LONGLONG llTime;

	header.ktr_type = KTR_PSIG;
	header.ktr_pid = Task->TaskId;
	header.ktr_tid = Task->ThreadId;
	if (win_clock_gettime_MONOTONIC(&llTime)){
		ttime_posix(&header.ktr_time, &llTime);
	}
	win_strlcpy(header.ktr_comm, __PROGNAME, MAXCOMLEN);
	header.ktr_len = sizeof(struct ktr_psig);
	data.signo = signo;
	data.action = action;
	data.mask = Task->ProcMask;
	data.code = 0;		/* machine dependent signal code: <sys/siginfo.h> */
	data.si = *info;
	win_writev(Task->TraceHandle, iovData, 2, &ulResult);
}
void 
ktrace_GENIO(WIN_TASK *Task, int fd, enum uio_rw rw, const void *buffer, ssize_t size)
{
	struct ktr_header header = {0};
	struct ktr_genio data;
	WIN_IOVEC iovData[3] = {
		{(PVOID)&header, sizeof(struct ktr_header)},
		{(PVOID)&data, sizeof(struct ktr_genio)},
		{(PVOID)buffer, size}
	};
	ULONG ulResult;

	header.ktr_type = KTR_GENIO;
	header.ktr_pid = Task->TaskId;
	header.ktr_tid = Task->ThreadId;
	ttime_posix(&header.ktr_time, &Task->ClockTime);
	win_strlcpy(header.ktr_comm, __PROGNAME, MAXCOMLEN);
	header.ktr_len = sizeof(struct ktr_genio) + size;
	data.ktr_fd = fd;
	data.ktr_rw = rw;
	win_writev(Task->TraceHandle, iovData, 3, &ulResult);
}
void 
ktrace_STRUCT(WIN_TASK *Task, const char *name, size_t len, const void *data, size_t size)
{
	struct ktr_header header = {0};
	WIN_IOVEC iovData[3] = {
		{(PVOID)&header, sizeof(struct ktr_header)},
		{(PVOID)name, len + 1},
		{(PVOID)data, size}
	};
	ULONG ulResult;
	LONGLONG llTime;

	header.ktr_type = KTR_STRUCT;
	header.ktr_pid = Task->TaskId;
	header.ktr_tid = Task->ThreadId;
	header.ktr_len = len + 1 + size;
	if (win_clock_gettime_MONOTONIC(&llTime)){
		ttime_posix(&header.ktr_time, &llTime);
	}
	win_strlcpy(header.ktr_comm, __PROGNAME, MAXCOMLEN);
	win_writev(Task->TraceHandle, iovData, 3, &ulResult);
}
void 
ktrace_USER(WIN_TASK *Task, const char *label, void *addr, size_t len)
{
	struct ktr_header header = {0};
	struct ktr_user data;
	WIN_IOVEC iovData[3] = {
		{(PVOID)&header, sizeof(struct ktr_header)},
		{(PVOID)&data, sizeof(struct ktr_user)},
		{addr, len}
	};
	ULONG ulResult;
	LONGLONG llTime;

	header.ktr_type = KTR_USER;
	header.ktr_pid = Task->TaskId;
	header.ktr_tid = Task->ThreadId;
	header.ktr_len = sizeof(struct ktr_user) + len;
	if (win_clock_gettime_MONOTONIC(&llTime)){
		ttime_posix(&header.ktr_time, &llTime);
	}
	win_strlcpy(header.ktr_comm, __PROGNAME, MAXCOMLEN);
	win_strlcpy(data.ktr_id, label, KTR_USER_MAXIDLEN);
	win_writev(Task->TraceHandle, iovData, 3, &ulResult);
}

/****************************************************/

int 
ktrace_SET(WIN_TASK *Task , const char *tracefile, int trpoints)
{
	int result = 0;
	WIN_MODE wMode;
	struct ktr_header header = {0x0052544b, 0};	/* "KTR" */
	WIN_NAMEIDATA wPath;
	LONG lSize = sizeof(struct ktr_header);

	if (!vfs_ktrace_SET(Task, path_win(&wPath, tracefile, 0), &header, lSize)){
		result -= errno_posix(GetLastError());
	}else{
		Task->TracePoints = trpoints;
	}
	return(result);
}
int 
ktrace_CLEAR(WIN_TASK *Task)
{
	int result = 0;

	if (!vfs_ktrace_CLEAR(Task)){
		result -= errno_posix(GetLastError());
	}else{
		Task->TracePoints = 0;
	}
	return(result);
}

/****************************************************/

int 
sys_ktrace(call_t call, const char *tracefile, int ops, int trpoints, pid_t pid)
{
	int result = 0;

	if (ops & KTRFLAG_DESCEND){
		ops &= ~KTRFLAG_DESCEND;
		trpoints |= KTRFAC_INHERIT;
	}
	switch (ops){
		case KTROP_SET:
			result = ktrace_SET(call.Task, tracefile, trpoints);
			break;
		case KTROP_CLEAR:
			result = ktrace_CLEAR(call.Task);
			break;
		default:
			result = -EOPNOTSUPP;
	}
	return(result);
}
int 
sys_utrace(call_t call, const char *label, void *addr, size_t len)
{
	int result = 0;
	WIN_TASK *pwTask = call.Task;

	if (!(pwTask->TracePoints & KTRFAC_USER)){
		return(0);
	}else if (strlen(label) >= KTR_USER_MAXIDLEN){
		result = -ENAMETOOLONG;
	}else if (len > KTR_USER_MAXLEN){
		result = -EINVAL;
	}else{
		ktrace_USER(pwTask, label, addr, len);
	}
	return(result);
}
