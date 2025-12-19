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

#include <sys/select.h>

/****************************************************/

void 
fdset_init(struct pollfd pollfds[])
{
	int fd = 0;

	while (fd < WSA_MAXIMUM_WAIT_EVENTS){
		pollfds->fd = -1;
		fd++;
		pollfds++;
	}
}
void 
fdset_poll(WIN_TASK *Task, struct pollfd pollfds[], fd_set *restrict fds, short flags)
{
	int fd = 0;

	if (Task->TracePoints & KTRFAC_STRUCT){
		ktrace_STRUCT(Task, "fdset", 5, fds, sizeof(fd_set));
	}
	while (fd < WSA_MAXIMUM_WAIT_EVENTS){
		if (FD_ISSET(fd, fds)){
			pollfds->fd = fd;
			pollfds->events |= flags;
		}
		fd++;
		pollfds++;
	}
}
int 
fdset_select(WIN_TASK *Task, fd_set *restrict fds, struct pollfd pollfds[], short flags)
{
	int fd = 0;
	int count = 0;

	while (fd < WSA_MAXIMUM_WAIT_EVENTS){
		if (pollfds->revents & flags){
			count++;
		}else{
			FD_CLR(fd, fds);
		}
		fd++;
		pollfds++;
	}
	if (Task->TracePoints & KTRFAC_STRUCT){
		ktrace_STRUCT(Task, "fdset", 5, fds, sizeof(fd_set));
	}
	return(count);
}
void 
stime_posix(struct timeval *tp, DWORD Millisecs)
{
	LONGLONG llTime = (LONGLONG)Millisecs;

	llTime *= 1000;						/* microseconds */
	if (llTime > 0){
		tp->tv_sec = (time_t)(llTime * 0.000001);	/* __int64_t (%I64d) */
		tp->tv_usec = llTime - (tp->tv_sec * 1000000);
	}else{
		tp->tv_sec = 0LL;
		tp->tv_usec = 0;
	}
}

/****************************************************/

int 
__select(WIN_TASK *Task, int nfds, fd_set *restrict readfds, fd_set *restrict writefds, 
	fd_set *restrict errorfds, struct timeval *restrict timeout)
{
	int result = 0;
	WIN_POLLFD *fdVector[WSA_MAXIMUM_WAIT_EVENTS + 1];
	WIN_VNODE *vnVector[WSA_MAXIMUM_WAIT_EVENTS + 1];
	struct pollfd fds[WSA_MAXIMUM_WAIT_EVENTS] = {0};
	DWORD dwResult = 0;
	DWORD dwTimeOut = INFINITE;
	u_int size = howmany(nfds, NFDBITS);

	fdset_init(fds);
	if (timeout){
		dwTimeOut = timeout->tv_usec * 0.001;	/* microseconds */
		dwTimeOut += timeout->tv_sec * 1000;	/* __int64_t (%I64d) */
	}
	if (readfds){
		fdset_poll(Task, fds, readfds, (POLLIN | POLLHUP | POLLRDBAND));
	}
	if (writefds){
		fdset_poll(Task, fds, writefds, (POLLOUT | POLLWRBAND));
	}
	if (errorfds){
		fdset_poll(Task, fds, errorfds, (POLLERR | POLLNVAL));
	}
	/* Silently limit to WS maximum (ab.exe)
	 */
	if (!pollfd_win(Task, vnVector, fdVector, fds, WSA_MAXIMUM_WAIT_EVENTS)){
		return(-EINVAL);
	}else if (!vfs_poll(Task, vnVector, fdVector, &dwTimeOut, &dwResult)){
		return(-errno_posix(GetLastError()));
	}
	if (readfds){
		result += fdset_select(Task, readfds, fds, (POLLIN | POLLHUP | POLLRDBAND));
	}
	if (writefds){
		result += fdset_select(Task, writefds, fds, (POLLOUT | POLLWRBAND));
	}
	if (errorfds){
		result += fdset_select(Task, errorfds, fds, (POLLERR | POLLNVAL));
	}
	if (timeout){
		stime_posix(timeout, dwTimeOut);
	}
	return(result);
}
int 
sys_select(call_t call, int nfds, fd_set *restrict readfds, fd_set *restrict writefds, 
	fd_set *restrict errorfds, struct timeval *restrict timeout)
{
	return(__select(call.Task, nfds, readfds, writefds, errorfds, timeout));
}
int 
sys_pselect(call_t call, int nfds, fd_set *restrict readfds, fd_set *restrict writefds, 
	fd_set *restrict errorfds, const struct timespec *restrict timeout, 
	const sigset_t *restrict sigmask)
{
	struct timeval tv, *ptv = NULL;

	if (timeout){
		tv.tv_sec = timeout->tv_sec;
		tv.tv_usec = timeout->tv_nsec * 0.001;	/* microseconds */
		ptv = &tv;
	}
	return(__select(call.Task, nfds, readfds, writefds, errorfds, ptv));
}
