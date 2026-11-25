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

#include <sys/uio.h>

/************************************************************/

ssize_t 
sys_writev(call_t call, int fd, const struct iovec *iov, int iovcnt)
{
	ssize_t result = 0;
	ULONG ulResult;
	WIN_TASK *pwTask = call.Task;
	WIN_VNODE *pvNodes = __Nodes[pwTask->TaskId];

	if (iovcnt < 0 || iovcnt >= IOV_MAX){
		result = -EINVAL;
	}else if (fd < 0 || fd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_writev(&pvNodes[fd], (WIN_IOVEC *)iov, iovcnt, &ulResult)){
		result -= errno_posix(GetLastError());
	}else{
		result = ulResult;
	}
	return(result);
}
ssize_t 
sys_readv(call_t call, int fd, const struct iovec *iov, int iovcnt)
{
	ssize_t result = 0;
	ULONG ulResult;
	WIN_TASK *pwTask = call.Task;
	WIN_VNODE *pvNodes = __Nodes[pwTask->TaskId];

	if (iovcnt < 0 || iovcnt >= IOV_MAX){
		result = -EINVAL;
	}else if (fd < 0 || fd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_readv(pwTask, &pvNodes[fd], (WIN_IOVEC *)iov, iovcnt, &ulResult)){
		result -= errno_posix(GetLastError());
	}else{
		result = ulResult;
	}
	return(result);
}
