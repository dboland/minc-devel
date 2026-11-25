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

#include <sys/time.h>

/****************************************************/

struct timeval *
timeval_posix(struct timeval *tp, FILETIME *Time)
{
	LONGLONG llTime = *(LONGLONG *)Time;

	llTime -= 116444736000000000LL;			/* epoch */
	llTime *= 0.1;					/* microseconds */
	tp->tv_sec = (time_t)(llTime * 0.000001);
	tp->tv_usec = llTime - (tp->tv_sec * 1000000);
	return(tp);
}
FILETIME 
timeval_win(const struct timeval *timev)
{
	DWORDLONG dwlTime = (DWORDLONG)timev->tv_sec;
	FILETIME ftResult;

	dwlTime *= 10000000LL;				/* 100-nanosecond intervals */
	dwlTime += 116444736000000000LL;		/* epoch */
	dwlTime += (timev->tv_usec * 10);		/* microseconds */
	*(DWORDLONG *)&ftResult = dwlTime;
	return(ftResult);
}
FILETIME *
utimeval_win(FILETIME FileTime[2], const struct timeval tv[2])
{
	FILETIME fTime;

	if (!tv){
		GetSystemTimeAsFileTime(&fTime);
		FileTime[0] = fTime;
		FileTime[1] = fTime;
	}else{
		/* cp.exe -p */
		FileTime[0] = timeval_win(&tv[0]);	/* access */
		FileTime[1] = timeval_win(&tv[1]);	/* mod */
	}
	return(FileTime);
}
FILETIME 
timespec_win(const struct timespec *times)
{
	DWORDLONG dwlTime = times->tv_sec;
	FILETIME ftResult = {0, 0};

	if (times->tv_nsec == UTIME_NOW){
		GetSystemTimeAsFileTime(&ftResult);
	}else if (times->tv_nsec != UTIME_OMIT){
		dwlTime *= 10000000LL;			/* 100-nanosecond intervals */
		dwlTime += 116444736000000000LL;	/* epoch */
		dwlTime += (times->tv_nsec * 0.1);
		*(DWORDLONG *)&ftResult = dwlTime;
	}
	return(ftResult);
}
FILETIME *
utimespec_win(FILETIME FileTime[2], const struct timespec times[2])
{
	FILETIME fTime;

	if (!times){
		GetSystemTimeAsFileTime(&fTime);
		FileTime[0] = fTime;
		FileTime[1] = fTime;
	}else{
		FileTime[0] = timespec_win(&times[0]);
		FileTime[1] = timespec_win(&times[1]);
	}
	return(FileTime);
}
void 
itimeval_posix(struct timeval *tp, LONGLONG TimeOut)
{
	TimeOut *= 0.001;				/* microseconds */
	tp->tv_sec = (time_t)(TimeOut * 0.000001);
	tp->tv_usec = TimeOut - (tp->tv_sec * 1000000);
}

/****************************************************/

int 
clock_gettime_REALTIME(struct timespec *tp)
{
	LONGLONG llTime;

	GetSystemTimeAsFileTime((FILETIME *)&llTime);
	llTime -= 116444736000000000LL;			/* epoch (100-nanosecond intervals) */
	llTime *= 100;					/* nanoseconds */
	tp->tv_sec = (time_t)(llTime * 0.000000001);	/* seconds (gcc needs cast!) */
	tp->tv_nsec = llTime - (tp->tv_sec * 1000000000);
	return(0);
}
int 
clock_gettime_MONOTONIC(struct timespec *tp)
{
	int result = 0;
	LONGLONG llTime;

	if (!win_clock_gettime_MONOTONIC(&llTime)){	/* nanoseconds */
		result -= errno_posix(GetLastError());
	}else{
		tp->tv_sec = (time_t)(llTime * 0.000000001);		/* seconds (gcc needs cast!) */
		tp->tv_nsec = llTime - (tp->tv_sec * 1000000000);	/* nanoseconds */
	}
	return(result);
}
int 
setitimer_REAL(WIN_TASK *Task, LONG *Interval, DWORDLONG *TimeOut)
{
	int result = 0;

	if (!vfs_setitimer(Task, Interval, TimeOut)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}

/****************************************************/

int 
sys_nanosleep(call_t call, const struct timespec *req, struct timespec *rem)
{
	int result = 0;
	struct timespec remain;
	DWORDLONG dwlTimeOut = 0LL;
	WIN_TASK *pwTask = call.Task;

	if (!req){
		return(-EFAULT);
	}else if (req->tv_nsec < 0 || req->tv_nsec > 1000000000){
		return(-EINVAL);
	}else{
		dwlTimeOut = (DWORDLONG)(req->tv_sec * 1000000000);
		dwlTimeOut += req->tv_nsec;
	}
	if (!vfs_nanosleep(pwTask, dwlTimeOut, &dwlTimeOut)){
		result -= errno_posix(GetLastError());
	}
	remain.tv_sec = (time_t)(dwlTimeOut * 0.000000001);
	remain.tv_nsec = dwlTimeOut - (remain.tv_sec * 1000000000);	/* nanoseconds */
	if (pwTask->TracePoints & KTRFAC_STRUCT){
		ktrace_STRUCT(pwTask, "reltimespec", 11, &remain, sizeof(struct timespec));
	}
	if (rem){
		*rem = remain;
	}
	return(result);
}
int 
sys_gettimeofday(call_t call, struct timeval *tv, struct timezone *tz)
{
	int result = 0;
	DWORDLONG dwlTime;

	if (!tv){
		result = -EFAULT;
	}else{
		GetSystemTimeAsFileTime((FILETIME *)&dwlTime);
		dwlTime -= 116444736000000000LL;		/* epoch */
		dwlTime *= 0.1;					/* microseconds */
		tv->tv_sec = (time_t)(dwlTime * 0.000001);	/* seconds (date.exe) */
		tv->tv_usec = dwlTime - (tv->tv_sec * 1000000);	/* ab.exe */
	}
	return(result);
}
int 
sys_settimeofday(call_t call, const struct timeval *tp, const struct timezone *tzp)
{
	int result = 0;

	if (!tp){
		result = -EFAULT;
	}else if (!win_settimeofday(tp->tv_sec, tp->tv_usec * 0.001)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_clock_gettime(call_t call, clockid_t clockid, struct timespec *tp)
{
	int result = 0;
	WIN_TASK *pwTask = call.Task;

	if (!tp){
		return(-EFAULT);
	}
	switch (clockid){
		case CLOCK_REALTIME:			/* git.exe */
			result = clock_gettime_REALTIME(tp);
			break;
		case CLOCK_MONOTONIC:			/* git.exe */
			result = clock_gettime_MONOTONIC(tp);
			break;
//		case CLOCK_VIRTUAL:
		case CLOCK_PROCESS_CPUTIME_ID:
		case CLOCK_THREAD_CPUTIME_ID:
		case CLOCK_UPTIME:
			return(-EOPNOTSUPP);
		default:
			return(-EINVAL);
	}
	if (pwTask->TracePoints & KTRFAC_STRUCT){
		ktrace_STRUCT(pwTask, "reltimespec", 11, tp, sizeof(struct timespec));
	}
	return(result);
}
int 
sys_setitimer(call_t call, int which, const struct itimerval *restrict value, struct itimerval *restrict ovalue)
{
	int result = 0;
	LONG lInterval = 0;
	DWORDLONG dwlTimeOut = 0LL;
	WIN_TASK *pwTask = call.Task;

	/* OpenBSD's profiler passes NULL */
	if (value){
		lInterval = (value->it_interval.tv_sec & 0xBFFFFFFF) * 1000;
		lInterval += value->it_interval.tv_usec * 0.001;
		dwlTimeOut = (DWORDLONG)(value->it_value.tv_usec * 1000);
		dwlTimeOut += (DWORDLONG)(value->it_value.tv_sec * 1000000000);
	}
	switch (which){
		case ITIMER_REAL:
			result = setitimer_REAL(pwTask, &lInterval, &dwlTimeOut);
			break;
		case ITIMER_VIRTUAL:
		case ITIMER_PROF:
			return(-EOPNOTSUPP);
		default:
			return(-EINVAL);
	}
	if (ovalue){
		itimeval_posix(&ovalue->it_value, dwlTimeOut);
		ovalue->it_interval.tv_sec = (time_t)(lInterval * 0.001);
		ovalue->it_interval.tv_usec = (lInterval % 1000) * 1000;
	}
	return(result);
}
int 
sys_futimens(call_t call, int fd, const struct timespec times[2])
{
	int result = 0;
	FILETIME fTime[2];
	WIN_TASK *pwTask = call.Task;
	WIN_VNODE *pvNodes = __Nodes[pwTask->TaskId];

	if (fd < 0 || fd >= OPEN_MAX){
		result = -EBADF;
	}else if (!disk_futimes(&pvNodes[fd], utimespec_win(fTime, times))){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
__utimensat(int dirfd, const char *path, FILETIME Times[2], int flags)
{
	int result = 0;
	WIN_NAMEIDATA wPath;

	if (!vfs_utimes(pathat_win(&wPath, dirfd, path, flags), Times)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_utimensat(call_t call, int dirfd, const char *file, const struct timespec times[2], int flag)
{
	FILETIME fTime[2];

	return(__utimensat(dirfd, file, utimespec_win(fTime, times), flag));
}
int 
sys_utimes(call_t call, const char *path, const struct timeval times[2])
{
	FILETIME fTime[2];

	return(__utimensat(AT_FDCWD, path, utimeval_win(fTime, times), 0));
}
int 
sys_futimes(call_t call, int fd, const struct timeval tv[2])
{
	int result = 0;
	FILETIME fTime[2];
	WIN_TASK *pwTask = call.Task;
	WIN_VNODE *pvNodes = __Nodes[pwTask->TaskId];

	if (fd < 0 || fd >= OPEN_MAX){
		result = -EBADF;
	}else if (!disk_futimes(&pvNodes[fd], utimeval_win(fTime, tv))){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
