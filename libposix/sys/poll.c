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

#include <poll.h>

/****************************************************/

BOOL 
pollfd_win(WIN_TASK *Task, WIN_VNODE *Nodes[], WIN_POLLFD *Info[], struct pollfd fds[], nfds_t nfds)
{
	nfds_t i = 0;
	int count = 0;
	int fd;
	WIN_VNODE *pNodes = Task->Node;

	if (nfds < 0 || nfds > WSA_MAXIMUM_WAIT_EVENTS){
		return(FALSE);
	}else while (i < nfds){
		fd = fds->fd;
		fds->revents = 0;
		if (fd >= OPEN_MAX){
			fds->revents = POLLNVAL;
		}else if (fd >= 0 && fds->events && pNodes[fd].Access){
			Nodes[count] = &pNodes[fd];
			Info[count] = (WIN_POLLFD *)fds;
			count++;
		}
		i++;
		fds++;
	}
	Nodes[count] = NULL;
	Info[count] = NULL;
	return(TRUE);
}

/****************************************************/

int 
__poll(WIN_TASK *Task, struct pollfd fds[], nfds_t nfds, DWORD *TimeOut)
{
	int result = 0;
	WIN_POLLFD *fdVector[WSA_MAXIMUM_WAIT_EVENTS + 1];
	WIN_VNODE *vnVector[WSA_MAXIMUM_WAIT_EVENTS + 1];
	DWORD dwResult = 0;

	if (!fds){
		result = -EFAULT;
	}else if (!pollfd_win(Task, vnVector, fdVector, fds, nfds)){
		result = -EINVAL;
	}else if (!vfs_poll(Task, vnVector, fdVector, TimeOut, &dwResult)){
		result -= errno_posix(GetLastError());
	}else if (dwResult){
		result = dwResult;
	}else if (*TimeOut){
		result = -EINTR;
	}
	if (Task->TracePoints & KTRFAC_STRUCT){
		ktrace_STRUCT(Task, "pollfd", 6, fds, sizeof(struct pollfd) * nfds);
	}
	return(result);
}
int 
sys_poll(call_t call, struct pollfd fds[], nfds_t nfds, int timeout)
{
	DWORD dwTimeOut = timeout;

	if (timeout < 0){
		dwTimeOut = INFINITE;
	}
	return(__poll(call.Task, fds, nfds, &dwTimeOut));
}
int
sys_ppoll(call_t call, struct pollfd *fds, nfds_t nfds, const struct timespec *timeout,
    const sigset_t *mask)
{
	DWORD dwTimeOut = INFINITE;

WIN_ERR("sys_ppoll(%d)\n", nfds);
	return(-EOPNOTSUPP);
}
