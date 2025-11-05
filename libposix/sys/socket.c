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

#include <sys/socket.h>

/****************************************************/

void 
sflags_debug(int flags, const char *lable)
{
	DWORD dwRemain = flags;

	WIN_ERR("  %s(0x%x): ", lable, dwRemain);
	win_flagname(MSG_OOB, "OOB", dwRemain, &dwRemain);
	win_flagname(MSG_DONTROUTE, "DONTROUTE", dwRemain, &dwRemain);
	win_flagname(MSG_NOSIGNAL, "NOSIGNAL", dwRemain, &dwRemain);
	win_flagname(MSG_DONTWAIT, "DONTWAIT", dwRemain, &dwRemain);

	WIN_ERR(" remain(0x%x)\n", dwRemain);
}
void 
saddr_debug(const struct sockaddr *addr, socklen_t addrlen, const char *label)
{
	int i = 0;

	if (!addr){
		return;
	}
	WIN_ERR("%s(): addrlen(%d) sa_family(%d) sa_len(%d) sa_data(", 
		label, addrlen, addr->sa_family, addr->sa_len);
	while (i < addrlen){
		WIN_ERR("[%d]", addr->sa_data[i++]);
	}
	WIN_ERR(")\n");
}

/****************************************************/

DWORD 
af_win(sa_family_t family)
{
	DWORD dwResult = family;

	if (family == AF_ROUTE){
		dwResult = WS_AF_ROUTE;
	}else if (family == AF_INET6){
		dwResult = WS_AF_INET6;
	}
	return(dwResult);
}
sa_family_t 
af_posix(DWORD Family)
{
	sa_family_t result = Family;

	if (Family == WS_AF_ROUTE){
		result = AF_ROUTE;
	}else if (Family == WS_AF_INET6){
		result = AF_INET6;
	}
	return(result);
}
void 
so_sockopt_win(WIN_TASK *Task, struct cmsghdr *cmsg)
{
	int fd;
	WIN_VNODE vNode = {0};
	unsigned char *data = CMSG_DATA(cmsg);

	switch (cmsg->cmsg_type){
		case SCM_RIGHTS:			/* syslogd.exe */
			fd = *(int *)data;
			vfs_F_DUPFD(&Task->Node[fd], FALSE, &vNode);
			*(HANDLE *)data = vNode.Handle;
			break;
	}
}
void 
so_sockopt_posix(WIN_TASK *Task, struct cmsghdr *cmsg)
{
	HANDLE hChannel;
	WIN_VNODE vNode = {0};
	unsigned char *data = CMSG_DATA(cmsg);

	switch (cmsg->cmsg_type){
		case SCM_RIGHTS:			/* syslogd.exe */
			hChannel = *(HANDLE *)data;
			vfs_namei(hChannel, 1, &vNode);
			*(int *)data = fd_posix(Task, &vNode, 0);
			break;
		case SCM_TIMESTAMP:
WIN_ERR("SCM_TIMESTAMP:\n");
			break;
	}
}
int 
sockopt_win(int level, int name)
{
	switch (level){
		case IPPROTO_IP:
			name = IN_SOCKOPT_WIN[name];
			break;
		case IPPROTO_IPV6:
			name = IN6_SOCKOPT_WIN[name];
			break;
	}
	return(name);
}
struct sockaddr *
saddr_posix(WIN_TASK *Task, struct sockaddr *result, SOCKADDR *Address, UINT *Length)
{
	/* In a Windows Socket address the sa_len field is missing. This is
	 * compensated by adding 2 bytes at the end of the structure. So, in
	 * total the two structures end up the same size.
	 */
	if (!result){
		return(NULL);
	}
	result->sa_len = *Length;				/* uint8_t */
	result->sa_family = af_posix(Address->sa_family);	/* uint8_t */
	if (Address->sa_family == WS_AF_LOCAL){
		path_posix(result->sa_data, (LPWSTR)Address->sa_data);
	}else{
		win_memcpy(result->sa_data, Address->sa_data, *Length - 2);
	}
	if (Task->TracePoints & KTRFAC_STRUCT){
		ktrace_STRUCT(Task, "sockaddr", 8, result, result->sa_len);
	}
	return(result);
}
SOCKADDR *
saddr_win(WIN_TASK *Task, SOCKADDR *Result, const struct sockaddr *addr, const socklen_t *addrlen)
{
	WIN_NAMEIDATA wPath = {0};
	struct sockaddr *addrpeek = (void *)addr;

	if (!addr){
		return(NULL);
	}
	addrpeek->sa_len = *addrlen;		/* not always set (lynx.exe) */
	if (Task->TracePoints & KTRFAC_STRUCT){
		ktrace_STRUCT(Task, "sockaddr", 8, addr, *addrlen);
	}
	Result->sa_family = af_win(addr->sa_family);
	if (addr->sa_family == AF_LOCAL){
		path_win(&wPath, addr->sa_data, 0);
		win_wcslcpy((LPWSTR)Result->sa_data, wPath.Resolved, MAX_PATH);
	}else{
		win_memcpy(Result->sa_data, addr->sa_data, *addrlen - 2);
	}
	return(Result);
}
LPWSABUF
iovec_swap(struct iovec vec[], unsigned int count)
{
	size_t len;
	void *base;
	LPWSABUF pbResult = (LPWSABUF)vec;

	/* WinSock's message header (WSAMSG) is 99% compatible. Only the
	 * two fields in its iovec structure (WSABUF) need to be reversed.
	 */
	while (count--){
		len = vec->iov_len;
		base = vec->iov_base;
		vec->iov_len = (size_t)base;
		vec->iov_base = (void *)len;
		vec++;
	}
	return(pbResult);
}
WSAMSG *
msghdr_win(WIN_TASK *Task, WSAMSG *Result, const struct msghdr *msg, int flags)
{
	if (Task->TracePoints & KTRFAC_STRUCT){
		ktrace_STRUCT(Task, "msghdr", 6, msg, sizeof(struct msghdr));
	}
	Result->lpBuffers = iovec_swap(msg->msg_iov, msg->msg_iovlen);
	Result->dwBufferCount = msg->msg_iovlen;
	Result->Control.buf = msg->msg_control;		/* ancillary data */
	Result->Control.len = msg->msg_controllen;
	Result->dwFlags = flags;
	return(Result);
}
void 
cmsghdr_win(WIN_TASK *Task, const struct msghdr *msg)
{
	struct cmsghdr *cmsg = CMSG_FIRSTHDR(msg);
	int level;

	while (cmsg){
		level = cmsg->cmsg_level;
		if (level == SOL_SOCKET){
			so_sockopt_win(Task, cmsg);
		}else if (level == IPPROTO_IP){
			cmsg->cmsg_type = IN_SOCKOPT_WIN[cmsg->cmsg_type];
		}else if (level == IPPROTO_IPV6){
			cmsg->cmsg_type = IN6_SOCKOPT_WIN[cmsg->cmsg_type];
		}
		cmsg = CMSG_NXTHDR(msg, cmsg);
	}
}
void 
cmsghdr_posix(WIN_TASK *Task, const struct msghdr *msg)
{
	struct cmsghdr *cmsg = CMSG_FIRSTHDR(msg);
	int level;

	while (cmsg){
		level = cmsg->cmsg_level;
		if (level == SOL_SOCKET){
			so_sockopt_posix(Task, cmsg);
		}else if (level == IPPROTO_IP){
			cmsg->cmsg_type = IN_SOCKOPT_POSIX[cmsg->cmsg_type];
		}else if (level == IPPROTO_IPV6){
			cmsg->cmsg_type = IN6_SOCKOPT_POSIX[cmsg->cmsg_type];
		}
		cmsg = CMSG_NXTHDR(msg, cmsg);
	}
	if (Task->TracePoints & KTRFAC_STRUCT){
		ktrace_STRUCT(Task, "cmsghdr", 7, msg->msg_control, msg->msg_controllen);
	}
}

/****************************************************/

int 
sys_getrtable(call_t call)
{
	WIN_TASK *pwTask = call.Task;

	return(pwTask->SessionId);
}
int 
sys_socket(call_t call, int family, int type, int protocol)
{
	int result = 0;
	WIN_VNODE vNode = {0};
	WIN_TASK *pwTask = call.Task;

	if (!vfs_socket(af_win(family), type, protocol, &vNode)){
		result -= errno_posix(WSAGetLastError());
	}else{
		result = fd_posix(pwTask, &vNode, 0);
	}
	return(result);
}
int 
sys_connect(call_t call, int sockfd, const struct sockaddr *address, socklen_t addrlen)
{
	int result = 0;
	SOCKADDR sAddress = {0};
	WIN_TASK *pwTask = call.Task;

	if (sockfd < 0 || sockfd >= OPEN_MAX){	/* ab.exe */
		result = -EBADF;
	}else if (!vfs_connect(&pwTask->Node[sockfd], saddr_win(pwTask, &sAddress, address, &addrlen), addrlen)){
		result -= errno_posix(WSAGetLastError());
	}
	return(result);
}
int 
sys_bind(call_t call, int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	int result = 0;
	SOCKADDR sAddress = {0};
	WIN_TASK *pwTask = call.Task;

	if (sockfd < 0 || sockfd >= OPEN_MAX){	/* sshd.exe */
		result = -EBADF;
	}else if (!vfs_bind(pwTask, &pwTask->Node[sockfd], saddr_win(pwTask, &sAddress, addr, &addrlen), addrlen)){
		result -= errno_posix(WSAGetLastError());
	}
	return(result);
}
int 
sys_accept(call_t call, int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	int result = 0;
	SOCKADDR sAddress = {0};
	WIN_VNODE vnResult = {0};
	WIN_TASK *pwTask = call.Task;

	if (sockfd < 0 || sockfd >= OPEN_MAX){	/* ab.exe */
		result = -EBADF;
	}else if (!vfs_accept(pwTask, &pwTask->Node[sockfd], &sAddress, addrlen, &vnResult)){
		result -= errno_posix(WSAGetLastError());
	}else{
		saddr_posix(pwTask, addr, &sAddress, addrlen);
		result = fd_posix(pwTask, &vnResult, 0);
	}
	return(result);
}
ssize_t 
sys_recvfrom(call_t call, int sockfd, void *buf, size_t size, int flags, 
	struct sockaddr *src_addr, socklen_t *addrlen)
{
	ssize_t result = 0;
	DWORD dwResult = 0;
	SOCKADDR sAddress = {0};
	WIN_TASK *pwTask = call.Task;

	if (sockfd < 0 || sockfd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_recvfrom(&pwTask->Node[sockfd], buf, size, flags, saddr_win(pwTask, &sAddress, src_addr, addrlen), addrlen, &dwResult)){
		result -= errno_posix(WSAGetLastError());
	}else{
		saddr_posix(pwTask, src_addr, &sAddress, addrlen);
		if (pwTask->TracePoints & KTRFAC_GENIO){
			ktrace_GENIO(pwTask, sockfd, UIO_READ, buf, dwResult);
		}
		result = dwResult;
	}
	return(result);
}
ssize_t 
sys_sendto(call_t call, int sockfd, const void *buf, size_t size, int flags, 
	const struct sockaddr *dest_addr, socklen_t addrlen)
{
	ssize_t result = 0;
	DWORD dwResult = 0;
	SOCKADDR sAddress = {0};
	WIN_TASK *pwTask = call.Task;

	if (pwTask->TracePoints & KTRFAC_GENIO){
		ktrace_GENIO(pwTask, sockfd, UIO_WRITE, buf, size);
	}
	if (sockfd < 0 || sockfd >= OPEN_MAX){	/* sshd.exe */
		result = -EBADF;
	}else if (!vfs_sendto(&pwTask->Node[sockfd], buf, size, flags, saddr_win(pwTask, &sAddress, dest_addr, &addrlen), addrlen, &dwResult)){
		result -= errno_posix(WSAGetLastError());
	}else{
		result = dwResult;
	}
	return(result);
}
ssize_t 
sys_sendmsg(call_t call, int sockfd, const struct msghdr *msg, int flags)
{
	ssize_t result = 0;
	WSAMSG wMessage = {0};
	SOCKADDR sAddress = {0};
	DWORD dwResult;
	WIN_TASK *pwTask = call.Task;
	DWORD dwFlags = flags;

	wMessage.name = saddr_win(pwTask, &sAddress, msg->msg_name, &msg->msg_namelen);
	wMessage.namelen = msg->msg_namelen;
	cmsghdr_win(pwTask, msg);
	if (sockfd < 0 || sockfd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_sendmsg(&pwTask->Node[sockfd], msghdr_win(pwTask, &wMessage, msg, flags), &dwResult)){
		result -= errno_posix(WSAGetLastError());
	}else{
		/* swap back iovec pointers */
		iovec_swap(msg->msg_iov, msg->msg_iovlen);
		cmsghdr_posix(pwTask, msg);
		result = dwResult;
	}
	return(result);
}
ssize_t 
sys_recvmsg(call_t call, int sockfd, struct msghdr *msg, int flags)
{
	ssize_t result = 0;
	SOCKADDR sAddress = {0};
	WSAMSG wMessage = {0};
	DWORD dwResult;
	WIN_TASK *pwTask = call.Task;
	DWORD dwFlags = flags;

	wMessage.name = &sAddress;
	wMessage.namelen = msg->msg_namelen;	/* used for in/out (dig.exe) */
	if (sockfd < 0 || sockfd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_recvmsg(&pwTask->Node[sockfd], msghdr_win(pwTask, &wMessage, msg, flags), &dwResult)){
		result -= errno_posix(WSAGetLastError());
	}else{
		msg->msg_controllen = wMessage.Control.len;
		/* swap back iovec pointers */
		iovec_swap(msg->msg_iov, msg->msg_iovlen);
		saddr_posix(pwTask, msg->msg_name, wMessage.name, &msg->msg_namelen);
		cmsghdr_posix(pwTask, msg);
		result = dwResult;
	}
	return(result);
}
int 
sys_socketpair(call_t call, int domain, int type, int protocol, int sv[2])
{
	int result = 0;
	WIN_VNODE vnResult[2] = {0};
	WIN_TASK *pwTask = call.Task;
	DWORD dwAttribs = PIPE_TYPE_BYTE | PIPE_READMODE_BYTE;

	if (type == SOCK_DGRAM){
		dwAttribs = PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE;
	}
	if (!vfs_socketpair(af_win(domain), dwAttribs, protocol, vnResult)){
		result -= errno_posix(WSAGetLastError());
	}else{
		sv[0] = fd_posix(pwTask, &vnResult[0], 0);
		sv[1] = fd_posix(pwTask, &vnResult[1], 0);
		if (pwTask->TracePoints & KTRFAC_STRUCT){
			ktrace_STRUCT(pwTask, "fdvec", 5, sv, (sizeof(int) * 2));
		}
	}
	return(result);
}
int 
sys_shutdown(call_t call, int sockfd, int how)
{
	int result = 0;
	WIN_TASK *pwTask = call.Task;

	if (sockfd < 0 || sockfd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_shutdown(&pwTask->Node[sockfd], how)){
		result -= errno_posix(WSAGetLastError());
	}
	return(result);
}
int 
sys_getsockopt(call_t call, int sockfd, int level, int optname, void *optval, socklen_t *optlen)
{
	int result = 0;
	WIN_TASK *pwTask = call.Task;

	if (sockfd < 0 || sockfd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_getsockopt(&pwTask->Node[sockfd], level, optname, optval, optlen)){
		result -= errno_posix(WSAGetLastError());
	}else if (level == SOL_SOCKET && optname == SO_ERROR){	/* perl.exe */
		*(int *)optval = errno_posix(*(DWORD *)optval);
	}
	return(result);
}
int 
sys_setsockopt(call_t call, int sockfd, int level, int optname, const void *optval, socklen_t optlen)
{
	int result = 0;
	WIN_TASK *pwTask = call.Task;

	if (sockfd < 0 || sockfd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_setsockopt(&pwTask->Node[sockfd], level, sockopt_win(level, optname), optval, optlen)){
		result -= errno_posix(WSAGetLastError());
	}
	return(result);
}
int 
sys_listen(call_t call, int sockfd, int backlog)
{
	int result = 0;
	WIN_TASK *pwTask = call.Task;

	if (sockfd < 0 || sockfd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_listen(&pwTask->Node[sockfd], backlog)){
		result -= errno_posix(WSAGetLastError());
	}
	return(result);
}
int 
sys_getpeername(call_t call, int sockfd, struct sockaddr *address, socklen_t *address_len)
{
	int result = 0;
	SOCKADDR sAddress = {0};
	WIN_TASK *pwTask = call.Task;

	if (sockfd < 0 || sockfd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_getpeername(&pwTask->Node[sockfd], &sAddress, address_len)){
		result -= errno_posix(WSAGetLastError());
	}else{
		saddr_posix(pwTask, address, &sAddress, address_len);
	}
	return(result);
}
int 
sys_getsockname(call_t call, int sockfd, struct sockaddr *address, socklen_t *address_len)
{
	int result = 0;
	SOCKADDR sAddress = {0};
	WIN_TASK *pwTask = call.Task;

	if (sockfd < 0 || sockfd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_getsockname(&pwTask->Node[sockfd], &sAddress, address_len)){
		result -= errno_posix(WSAGetLastError());
	}else{
		saddr_posix(pwTask, address, &sAddress, address_len);
	}
	return(result);
}
