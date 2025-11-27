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

#include <sys/fcntl.h>

/****************************************************/

WIN_FLAGS *
flags_win(WIN_FLAGS *Result, int flags)
{
	Result->Access = GENERIC_READ;
	Result->Share = FILE_SHARE_READ | FILE_SHARE_WRITE;
	Result->Creation = OPEN_EXISTING;
	Result->Attribs = FILE_ATTRIBUTE_NORMAL;
	Result->CloseExec = FALSE;

	if (flags & O_RDWR){
		Result->Access |= GENERIC_WRITE;
	}else if (flags & O_APPEND){			/* mutt.exe (save) */
		Result->Access |= FILE_APPEND_DATA;
	}else if (flags & O_WRONLY){
		Result->Access = GENERIC_WRITE;
	}
	if (flags & O_CLOEXEC){
		Result->CloseExec = TRUE;
	}
//	if (flags & O_NONBLOCK){	/* GNU tar.exe */
//		Result->Attribs |= FILE_FLAG_OVERLAPPED;
//	}
	if (flags & O_CREAT){
		if (flags & O_EXCL){
			Result->Creation = CREATE_NEW;
		}else if (flags & O_TRUNC){
			Result->Creation = CREATE_ALWAYS;
		}else{
			Result->Creation = OPEN_ALWAYS;
		}
	}else if (flags & O_TRUNC){
		Result->Creation = TRUNCATE_EXISTING;
	}
	return(Result);
}
int 
flflags_posix(WIN_VNODE *Node)
{
	int result = O_RDONLY;
	DWORD dwAccess = Node->Access;
	DWORD dwAttribs = Node->Attribs;

	if (dwAccess & FILE_WRITE_DATA){
		if (dwAccess & FILE_READ_DATA){
			result = O_RDWR;
		}else{
			result = O_WRONLY;
		}
	}else if (dwAccess & FILE_APPEND_DATA){		/* why always set? */
		result = O_WRONLY | O_APPEND;
	}
	if (Node->CloseExec){
		result |= O_CLOEXEC;
	}
	if (dwAttribs & FILE_FLAG_OVERLAPPED){
		result |= O_NONBLOCK;
	}
//	if (dwAttribs & FILE_FLAG_BACKUP_SEMANTICS){
//		result |= O_DIRECTORY;
//	}
	return(result);
}
WIN_FLAGS * 
flflags_win(WIN_FLAGS *Result, int flags)
{
	Result->Access = WIN_S_IREAD;
	Result->Attribs = FILE_ATTRIBUTE_NORMAL;
	Result->CloseExec = FALSE;

	if (flags & O_RDWR){
		Result->Access |= WIN_S_IWRITE;
	}else if (flags & O_WRONLY){
		Result->Access = WIN_S_IWRITE;
	}
	if (flags & O_APPEND){
		Result->Access |= FILE_APPEND_DATA;
	}
	if (flags & O_CLOEXEC){
		Result->CloseExec = TRUE;
	}
	if (flags & O_NONBLOCK){
		Result->Attribs |= FILE_FLAG_OVERLAPPED;
	}
//	if (flags & O_DIRECTORY){
//		Result->Attribs |= FILE_FLAG_BACKUP_SEMANTICS;
//	}
	return(Result);
}
int 
fdflags_posix(WIN_VNODE *Node)
{
	int result = 0;

	if (Node->CloseExec){
		result |= FD_CLOEXEC;
	}
	return(result);
}

/****************************************************/

int 
fcntl_F_DUPFD(WIN_TASK *Task, WIN_VNODE *Node, BOOL CloseExec, int offset)
{
	int result = 0;
	WIN_VNODE vNode = {0};

	if (offset < 0 || offset >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_F_DUPFD(Node, CloseExec, &vNode)){
		result -= errno_posix(GetLastError());
	}else{
		result = fd_posix(Task, &vNode, offset);
	}
	return(result);
}
int 
fcntl_F_SETFL(WIN_TASK *Task, WIN_VNODE *Node, int flags)
{
	int result = 0;
	WIN_FLAGS wFlags = {0};

	if (!vfs_F_SETFL(Node, flflags_win(&wFlags, flags))){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
fcntl_F_SETFD(WIN_VNODE *Node, int flags)
{
	if (flags & FD_CLOEXEC){
		Node->CloseExec = TRUE;
	}else{
		Node->CloseExec = FALSE;
	}
	return(0);
}
int 
fcntl_F_SETLK(WIN_TASK *Task, WIN_VNODE *Node, BOOL Wait, struct flock *lock)
{
	int result = 0;
	DWORD dwFlags = LOCKFILE_FAIL_IMMEDIATELY;
	DWORDLONG dwlOffset = lock->l_start;
	DWORDLONG dwlSize = lock->l_len;

	if (Task->TracePoints & KTRFAC_STRUCT){
		ktrace_STRUCT(Task, "flock", 5, lock, sizeof(struct flock));
	}
	if (Wait){
		dwFlags = 0;
	}
	if (!dwlSize && Node->LockSize){
		dwlSize = Node->LockSize;
		dwlOffset = Node->LockRegion / dwlSize;
	}
	switch (lock->l_type){
		case F_RDLCK:		/* 1 */
			dwFlags |= LOCKFILE_SHARED;
			break;
		case F_UNLCK:		/* 2 */
			dwFlags = LOCKFILE_UNLOCK;
			break;
		case F_WRLCK:		/* 3 */
			dwFlags |= LOCKFILE_EXCLUSIVE_LOCK;
			break;
		default:
			return(-EINVAL);
	}
	if (!vfs_F_SETLK(Node, dwFlags, (LPLARGE_INTEGER)&dwlOffset, (LPLARGE_INTEGER)&dwlSize)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
fcntl_F_GETLK(WIN_VNODE *Node, struct flock *lock)
{
	int result = 0;

	if (!Node->LockSize){
		lock->l_type = F_UNLCK;
	}else{
		lock->l_start = Node->LockRegion / Node->LockSize;
		lock->l_len = Node->LockSize;
	}
	return(result);
}
int 
fcntl_F_SETOWN(WIN_TASK *Task, WIN_VNODE *Node, int owner)
{
	int result = 0;

	if (owner < 0 || owner >= CHILD_MAX){
		result = -EINVAL;
	}else{
		Node->Owner = owner;
	}
	return(result);
}

/****************************************************/

int 
sys_fcntl(call_t call, int fd, int cmd, ...)
{
	int result = 0;
	va_list args;
	WIN_TASK *pwTask = call.Task;
	WIN_VNODE *vNodes = pwTask->Node;

	va_start(args, cmd);
	if (fd < 0 || fd >= OPEN_MAX){
		result = -EBADF;
	}else switch (cmd){
		case F_DUPFD:
			result = fcntl_F_DUPFD(pwTask, &vNodes[fd], FALSE, va_arg(args, int));
			break;
		case F_DUPFD_CLOEXEC:
			result = fcntl_F_DUPFD(pwTask, &vNodes[fd], TRUE, va_arg(args, int));
			break;
		case F_SETFD:
			result = fcntl_F_SETFD(&vNodes[fd], va_arg(args, int));
			break;
		case F_GETFD:
			result = fdflags_posix(&vNodes[fd]);
			break;
		case F_SETFL:
			result = fcntl_F_SETFL(pwTask, &vNodes[fd], va_arg(args, int));
			break;
		case F_GETFL:
			result = flflags_posix(&vNodes[fd]);
			break;
		case F_SETLK:		/* makewhatis (mandoc.exe) */
			result = fcntl_F_SETLK(pwTask, &vNodes[fd], FALSE, va_arg(args, struct flock *));
			break;
		case F_SETLKW:
			result = fcntl_F_SETLK(pwTask, &vNodes[fd], TRUE, va_arg(args, struct flock *));
			break;
		case F_GETLK:
			result = fcntl_F_GETLK(&vNodes[fd], va_arg(args, struct flock *));
			break;
		case F_SETOWN:		/* see ioctl(2) */
			result = fcntl_F_SETOWN(pwTask, &vNodes[fd], va_arg(args, int));
			break;
		default:
			result = -EOPNOTSUPP;
	}
	va_end(args);
	return(result);
}
int 
__openat(WIN_TASK *Task, WIN_NAMEIDATA *Path, int flags, va_list args)
{
	int result = 0;
	mode_t mode = va_arg(args, mode_t);
	WIN_FLAGS wFlags;
	WIN_MODE wMode;
	WIN_VNODE vNode = {0};

	mode &= ~Task->FileMask;
	if ((Path->FileType == WIN_VLNK) && (Path->Flags == WIN_NOFOLLOW)){
		result = -ELOOP;
	}else if (!vfs_open(Path, flags_win(&wFlags, flags), mode_win(&wMode, mode), &vNode)){
		result -= errno_posix(GetLastError());
	}else{
		result = fd_posix(Task, &vNode, 0);
	}
	return(result);
}
int 
sys_open(call_t call, const char *path, int flags, ...)
{
	int result;
	va_list args;
	WIN_NAMEIDATA wPath = {0};

	if (!path[0]){
		return(-ENOENT);
	}
	va_start(args, flags);
	result = __openat(call.Task, path_win(&wPath, path, flags | O_INODE), flags, args);
	va_end(args);
	return(result);
}
int 
sys_openat(call_t call, int dirfd, const char *path, int flags, ...)
{
	int result;
	va_list args;
	WIN_NAMEIDATA wPath = {0};
	int atflags = AT_INODE;

	if (flags & O_NOFOLLOW){
		atflags |= AT_SYMLINK_NOFOLLOW;
	}
	if (flags & O_DIRECTORY){
		atflags |= AT_REMOVEDIR;
	}
	va_start(args, flags);
	result = __openat(call.Task, pathat_win(&wPath, dirfd, path, atflags), flags, args);
	va_end(args);
	return(result);
}
int 
sys_flock(call_t call, int fd, int operation)
{
	int result = 0;
	WIN_TASK *pwTask = call.Task;
	DWORD dwFlags = 0;
	LARGE_INTEGER liOffset = {0, 0};
	LARGE_INTEGER liSize = {0xFFFFFFFF, 0xFFFFFFFF};

	if (operation & LOCK_EX){
		dwFlags |= LOCKFILE_EXCLUSIVE_LOCK;
	}
	if (operation & LOCK_NB){
		dwFlags |= LOCKFILE_FAIL_IMMEDIATELY;
	}
	if (operation & LOCK_UN){
		dwFlags = LOCKFILE_UNLOCK;
	}
	if (fd < 0 || fd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_F_SETLK(&pwTask->Node[fd], dwFlags, &liOffset, &liSize)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
