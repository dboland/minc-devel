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

#include <sys/kernel.h>

/****************************************************/

long 
ticks_posix(FILETIME *Time)
{
	LONGLONG llTime = *(LONGLONG *)Time;

	llTime -= 116444736000000000LL;	/* epoch */
	llTime *= 0.0000001;		/* milliseconds */
	return((long)llTime);
}
u_int64_t
ticks64_posix(LARGE_INTEGER *Time)
{
	LONGLONG llTime = Time->QuadPart;

	llTime -= 116444736000000000LL;	/* epoch */
	llTime *= 0.0000001;		/* milliseconds */
	return(llTime);
}

/****************************************************/

int 
kern_KERN_CLOCKRATE(struct clockinfo *info)
{
	info->hz = win_KERN_CLOCKRATE();
	info->tick = 1000000000 / info->hz;
//	info->tickadj = 0;
	info->stathz = info->hz;
	info->profhz = info->hz;
	return(0);
}
int 
kern_KERN_HOSTNAME(char *curname, size_t *csize, const char *newname, size_t nsize)
{
	int result = 0;
	size_t size = 0;

	if (csize){
		size = *csize;
	}
	if (!win_KERN_HOSTNAME(curname, newname, size)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
kern_KERN_DOMAINNAME(char *curname, size_t *csize, const char *newname, size_t nsize)
{
	int result = 0;
	size_t size = 0;

	if (csize){
		size = *csize;
	}
	if (!win_KERN_DOMAINNAME(curname, newname, size)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
kern_KERN_PROC_OLD(const int *name, void *data, size_t *size)
{
	int result = 0;
	pid_t pid = WIN_PID_INIT;
	WIN_TASK *pwTask = &__Tasks[pid];

	/* sys/proc.c */

	while (pid < CHILD_MAX){
		if (pwTask->Flags){
			if (!data){
				*size += sizeof(struct kinfo_proc);
			}else{
				data = kproc_posix(data, pwTask);
			}
		}
		pid++;
		pwTask++;
	}
	return(result);
}
int 
kern_KERN_PROC(const int *name, void *data, size_t *size)
{
	int result = 0;
	pid_t pid = WIN_PID_INIT;
	WIN_TASK *pwTask = &__Tasks[pid];

	/* sys/proc.c */

	while (pid < CHILD_MAX){
		switch (name[2]){
			case KERN_PROC_KTHREAD:		/* kvm_getprocs(3): user-level plus kernel threads */
			case KERN_PROC_ALL:
				data = proc_KERN_PROC_ALL(pwTask, (struct kinfo_proc *)data, size);
				break;
			case KERN_PROC_PGRP:
				data = proc_KERN_PROC_PGRP(pwTask, name[3], (struct kinfo_proc *)data, size);
				break;
			case KERN_PROC_UID:
				data = proc_KERN_PROC_UID(pwTask, name[3], (struct kinfo_proc *)data, size);
				break;
			default:
				return(-EINVAL);
		}
		pid++;
		pwTask++;
	}
	return(result);
}
int 
kern_KERN_PROC_ARGS(const int *name, void *buf, size_t *size)
{
	int result = 0;
	pid_t pid = name[2];

	/* sys/proc.c */

	switch (name[3]){
		case KERN_PROC_ARGV:
			kargv_posix(pid, 1, buf);
			break;
		case KERN_PROC_NARGV:
		case KERN_PROC_ENV:
		case KERN_PROC_NENV:
		default:
			result = -ENOENT;
	}
	return(result);
}
int 
kern_KERN_CPTIME(long states[CPUSTATES])
{
	FILETIME ftIdle, ftKernel, ftUser;

	/* System time is not kernel time!
	 */
	GetSystemTimes(&ftIdle, &ftKernel, &ftUser);
	states[CP_USER] = ticks_posix(&ftUser);;
	states[CP_IDLE] = ticks_posix(&ftIdle);
	states[CP_SYS] = states[CP_USER] + ticks_posix(&ftKernel) - states[CP_IDLE];
	states[CP_NICE] = 0;
	states[CP_INTR] = 0;
	return(0);
}
int 
kern_KERN_CPTIME2(int cpu, u_int64_t states[CPUSTATES])
{
	int result = 0;
	LONG lCount = win_HW_NCPU();
	ULONG ulSize = sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION) * lCount;
	SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION *psppInfo = win_malloc(ulSize);

	if (cpu < 0 || cpu >= lCount){
		result = -EINVAL;
	}else if (!win_KERN_CPTIME2(psppInfo, ulSize)){
		result -= errno_posix(GetLastError());
	}else{
		psppInfo += cpu;
		states[CP_USER] = ticks64_posix(&psppInfo->UserTime);
		states[CP_IDLE] = ticks64_posix(&psppInfo->IdleTime);
		states[CP_SYS] = states[CP_USER] + ticks64_posix(&psppInfo->KernelTime) - states[CP_IDLE];
		states[CP_NICE] = 0;
		states[CP_INTR] = 0;
	}
	win_free(psppInfo);
	return(result);
}
int 
kern_KERN_VERSION(char *buf, size_t bufsize)
{
	int result = 0;
	int size = 0;
	struct tm *local;
	time_t time = BUILD;

	size = sprintf(buf, "OpenBSD %s (MINC): #%s: ", RELEASE, VERSION);
	if (!win_KERN_VERSION((DWORDLONG)BUILD, buf + size, bufsize - size)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
kern_KERN_SECURELVL(int *oldvalue, int *newvalue)
{
	int result = 0;

	if (oldvalue){
		*oldvalue = __Globals->SecureLevel;
	}else if (newvalue){
		__Globals->SecureLevel = *newvalue;
	}else{
		result = -EFAULT;
	}
	return(result);
}
int 
kern_KERN_ARND(char *buf, size_t size)
{
	int result = 0;
	DWORD dwResult;

	if (!rand_read(buf, size, &dwResult)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
kern_KERN_BOOTTIME(struct timeval *tv)
{
	int result = 0;
	FILETIME ftResult;

	if (!win_KERN_BOOTTIME(&ftResult)){
		result -= errno_posix(GetLastError());
	}else{
		timeval_posix(tv, &ftResult);
	}
	return(result);
}
