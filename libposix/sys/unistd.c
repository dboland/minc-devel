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

#include <unistd.h>

/*******************************************************/

long 
sys_pathconf(call_t call, const char *path, int name)
{
	long result = 0;

	switch (name){
		case _PC_MAX_CANON:
			result = MAX_CANON;
			break;
		case _PC_MAX_INPUT:
			result = WIN_MAX_INPUT;
			break;
//		case _PC_VDISABLE:
//			break;
		case _PC_NAME_MAX:
			result = WIN_NAME_MAX;
			break;
		case _PC_PATH_MAX:
			result = WIN_PATH_MAX;
			break;
		case _PC_PIPE_BUF:
			result = WIN_PIPE_BUF;
			break;
		default:
			result = -EOPNOTSUPP;
	}
	return(result);
}
long 
sys_fpathconf(call_t call, int fd, int name)
{
	return(sys_pathconf(call, "", name));
}
int 
sys_getdtablecount(call_t call)
{
	int result = 0;
	WIN_TASK *pwTask = call.Task;
	WIN_VNODE *pvNode = pwTask->Node;
	int fd = 0;

	while (fd < OPEN_MAX){
		if (pvNode->Access){
			result++;
		}
		fd++;
		pvNode++;
	}
	return(result);
}

/****************************************************/

int 
sys_getgroups(call_t call, int size, gid_t list[])
{
	int result = 0;
	SID8 *grList = NULL;
	DWORD dwCount = 0;
	int index = 0;

	if (size < 0){	/* GNU conftest.exe */
		result = -EINVAL;
	}else if (!win_getgroups(&grList, &dwCount)){
		result -= errno_posix(GetLastError());
	}else if (size >= dwCount){
		while (index < dwCount){
			list[index] = rid_posix(&grList[index]);
			if (list[index] == ROOT_GID){
				list[index] = 0;
			}
			index++;
		}
		result = dwCount;
	}
	win_free(grList);
	return(result);
}
int 
sys_setgroups(call_t call, int size, const gid_t *list)
{
	int result = 0;
	SID8 sid;
	SID8 *grList;
	DWORD dwIndex = 0;
	DWORD dwCount = size;
	WIN_CAP_CONTROL wControl;

	if (size < 0 || size >= NGROUPS_MAX){
		result = -EINVAL;
	}else{
		grList = win_malloc(sizeof(SID8) * dwCount);
		while (size--){
			if (!list[size]){
				grList[dwIndex] = *rid_win(&sid, ROOT_GID);
			}else{
				grList[dwIndex] = *rid_win(&sid, list[size]);
			}
			dwIndex++;
		}
		if (!win_cap_init(&wControl)){
			result -= errno_posix(GetLastError());
		}else if (!win_setgroups(&wControl, grList, dwCount)){
			result -= errno_posix(GetLastError());
		}
		win_cap_free(&wControl);
		win_free(grList);
	}
	return(result);
}

/****************************************************/

int 
sys_rmdir(call_t call, const char *pathname)
{
	int result = 0;
	int atflags = AT_SYMLINK_NOFOLLOW | AT_NOSLASH;
	WIN_NAMEIDATA wPath = {0};

	if (!vfs_rmdir(pathat_win(&wPath, AT_FDCWD, pathname, atflags))){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
__linkat(WIN_NAMEIDATA *Target, int fd2, const char *name2, int flag)
{
	int result = 0;
	WIN_NAMEIDATA wPath = {0};

	if (!vfs_link(Target, pathat_win(&wPath, fd2, name2, flag))){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_link(call_t call, const char *name1, const char *name2)
{
	WIN_NAMEIDATA wPath = {0};

	return(__linkat(path_win(&wPath, name1, 0), AT_FDCWD, name2, AT_SYMLINK_NOFOLLOW));
}
int 
sys_linkat(call_t call, int fd1, const char *name1, int fd2, const char *name2, int flag)
{
	int atflag = AT_SYMLINK_NOFOLLOW;
	WIN_NAMEIDATA wPath = {0};

	if (flag & AT_SYMLINK_FOLLOW){
		atflag = 0;
	}
	return(__linkat(pathat_win(&wPath, fd1, name1, atflag), fd2, name2, flag));
}
int 
__unlinkat(WIN_TASK *Task, int fd, const char *path, int flag)
{
	int result = 0;
	WIN_NAMEIDATA wPath = {0};
	BOOL bResult = FALSE;

	if (!path){
		result = -EFAULT;
	}else if (!*path){
		result = -ENOENT;
	}else if (flag & AT_REMOVEDIR){
		bResult = vfs_rmdir(pathat_win(&wPath, fd, path, AT_SYMLINK_NOFOLLOW));
	}else{
		bResult = vfs_unlink(pathat_win(&wPath, fd, path, AT_SYMLINK_NOFOLLOW));
	}
	if (!bResult){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_unlinkat(call_t call, int fd, const char *path, int flag)
{
	return(__unlinkat(call.Task, fd, path, flag));
}
int 
sys_unlink(call_t call, const char *path)
{
	return(__unlinkat(call.Task, AT_FDCWD, path, 0));
}
void 
sys_sync(call_t call)
{
	vfs_sync();
}
int 
sys_fsync(call_t call, int fd)
{
	int result = 0;
	WIN_TASK *pwTask = call.Task;

	if (fd < 0 || fd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_fsync(&pwTask->Node[fd])){
		result -= errno_posix(GetLastError());
	}else{
		result = 0;
	}
	return(result);
}
int 
sys_setdtablesize(call_t call, int newsize)
{
	WIN_ERR("  setdtablesize(%d)\n", newsize);
	return(0);
}
int 
sys___getcwd(call_t call, char *buf, size_t size)
{
	WIN_TASK *pwTask = call.Task;

	pathn_posix(buf, __Strings[pwTask->TaskId].Path, size);
	if (pwTask->TracePoints & KTRFAC_NAMEI){
		ktrace_NAMEI(pwTask, buf, strlen(buf));
	}
	return(0);
}
int 
sys_fchdir(call_t call, int fd)
{
	int result = 0;
	WIN_NAMEIDATA wPath = {0};
	WIN_TASK *pwTask = call.Task;

	if (!vfs_chdir(pwTask, pathat_win(&wPath, fd, "", 0))){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_chdir(call_t call, const char *path)
{
	int result = 0;
	WIN_NAMEIDATA wPath = {0};
	WIN_TASK *pwTask = call.Task;

	if (!vfs_chdir(pwTask, path_win(&wPath, path, O_NOSLASH))){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_chroot(call_t call, const char *path)
{
	int result = 0;
	WIN_NAMEIDATA wPath = {0};

	if (!path){
		result = -EFAULT;
	}else if (!path[0]){
		result = -ENOENT;
	}else if (!vfs_chroot(path_win(&wPath, path, 0))){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
ssize_t 
__readlinkat(WIN_TASK *Task, int dirfd, const char *path, char *buf, size_t bufsiz)
{
	ssize_t result = 0;
	int atflags = AT_SYMLINK | AT_SYMLINK_NOFOLLOW;
	WIN_NAMEIDATA wPath = {0};

	if (!path){
		result = -EFAULT;
	}else if (!path[0]){
		result = -ENOENT;
	}else if (!vfs_readlink(pathat_win(&wPath, dirfd, path, atflags))){
		result -= errno_posix(GetLastError());
	}else{
		result = pathnp_posix(buf, wPath.Resolved, bufsiz, TRUE) - buf;
	}
	return(result);
}
ssize_t 
sys_readlinkat(call_t call, int dirfd, const char *path, char *buf, size_t bufsiz)
{
	return(__readlinkat(call.Task, dirfd, path, buf, bufsiz));
}
int 
sys_readlink(call_t call, const char *path, char *buf, size_t bufsiz)
{
	return(__readlinkat(call.Task, AT_FDCWD, path, buf, bufsiz));
}
int 
__symlinkat(WIN_NAMEIDATA *Target, int fd, const char *path)
{
	int result = 0;
	int atflags = AT_SYMLINK_NOFOLLOW;
	WIN_NAMEIDATA wPath = {0};
	WIN_MODE wMode;

	if (!path){
		result = -EFAULT;
	}else if (!path[0]){
		result = -ENOENT;
	}else if (!vfs_symlink(pathat_win(&wPath, fd, path, atflags), mode_win(&wMode, 00755), Target)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_symlinkat(call_t call, const char *name1, int fd, const char *name2)
{
	WIN_NAMEIDATA wPath = {0};

	return(__symlinkat(pathat_win(&wPath, 0, name1, AT_NOSLASH), fd, name2));
}
int 
sys_symlink(call_t call, const char *name1, const char *name2)
{
	WIN_NAMEIDATA wPath = {0};

	return(__symlinkat(pathat_win(&wPath, 0, name1, AT_NOSLASH), AT_FDCWD, name2));
}
int 
__faccessat(int dirfd, const char *path, int mode, int flags)
{
	int result = 0;
	WIN_NAMEIDATA wPath = {0};
	ACCESS_MASK amDesired = 0;

	if (mode & R_OK){
		amDesired |= WIN_S_IREAD;
	}
	if (mode & W_OK){
		amDesired |= WIN_S_IWRITE;
	}
	if (mode & X_OK){
		amDesired |= WIN_S_IEXEC;
	}
	if (!vfs_access(pathat_win(&wPath, dirfd, path, flags), amDesired)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_faccessat(call_t call, int dirfd, const char *path, int mode, int flags)
{
	return(__faccessat(dirfd, path, mode, flags));
}
int 
sys_access(call_t call, const char *path, int mode)
{
	return(__faccessat(AT_FDCWD, path, mode, 0));
}
int 
__renameat(WIN_NAMEIDATA *Path, int tofd, const char *to)
{
	int result = 0;
	WIN_NAMEIDATA wPath = {0};

	if (Path->Attribs == -1){
		result -= errno_posix(GetLastError());
	}else if (!vfs_rename(Path, pathat_win(&wPath, tofd, to, AT_SYMLINK_NOFOLLOW))){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_renameat(call_t call, int fromfd, const char *from, int tofd, const char *to)
{
	int atflags = AT_SYMLINK_NOFOLLOW;
	WIN_NAMEIDATA wPath = {0};

	return(__renameat(pathat_win(&wPath, fromfd, from, atflags), tofd, to));
}
int 
sys_rename(call_t call, const char *from, const char *to)
{
	int atflags = AT_SYMLINK_NOFOLLOW;
	WIN_NAMEIDATA wPath = {0};

	return(__renameat(pathat_win(&wPath, AT_FDCWD, from, atflags), AT_FDCWD, to));
}
int 
sys_ftruncate(call_t call, int fd, off_t length)
{
	int result = 0;
	WIN_TASK *pwTask = call.Task;
	LARGE_INTEGER liOffset = {length & 0xFFFFFFFF, length >> 32};

	if (fd < 0 || fd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_ftruncate(&pwTask->Node[fd], &liOffset)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_truncate(call_t call, const char *path, off_t length)
{
	int result = 0;
	WIN_NAMEIDATA wPath = {0};
	LARGE_INTEGER liOffset = {length & 0xFFFFFFFF, length >> 32};

	if (!vfs_truncate(path_win(&wPath, path, 0), &liOffset)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}

/****************************************************/

int 
sys_dup(call_t call, int oldfd)
{
	int result = 0;
	WIN_VNODE vnResult = {0};
	WIN_TASK *pwTask = call.Task;

	if (oldfd < 0 || oldfd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_dup(&pwTask->Node[oldfd], &vnResult)){
		result -= errno_posix(GetLastError());
	}else{
		result = fd_posix(pwTask, &vnResult, 0);
	}
	return(result);
}
int 
sys_dup2(call_t call, int oldfd, int newfd)
{
	int result = 0;
	WIN_TASK *pwTask = call.Task;
	WIN_VNODE *pvNodes = pwTask->Node;

	if (oldfd < 0 || oldfd >= OPEN_MAX){
		result = -EBADF;
	}else if (newfd < 0 || newfd >= OPEN_MAX){
		result = -EBADF;			/* GNU conftest */
	}else if (!vfs_dup2(&pvNodes[oldfd], &pvNodes[newfd])){
		result -= errno_posix(GetLastError());
	}else{
		result = newfd;
	}
	return(result);
}
off_t 
sys_lseek(call_t call, int fd, off_t offset, int whence)
{
	off_t result = 0LL;
	DWORD dwMethod = FILE_BEGIN;
	LARGE_INTEGER liResult;
	WIN_TASK *pwTask = call.Task;
	LARGE_INTEGER liOffset = {offset & 0xFFFFFFFF, offset >> 32};

	if (whence == SEEK_CUR){
		dwMethod = FILE_CURRENT;
	}else if (whence == SEEK_END){
		dwMethod = FILE_END;
	}
	if (fd < 0 || fd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_lseek(&pwTask->Node[fd], &liOffset, dwMethod, &liResult)){
		result -= errno_posix(GetLastError());
	}else{
		result = liResult.QuadPart;
	}
	return(result);
}
int 
sys_close(call_t call, int fd)
{
	int result = 0;
	WIN_TASK *pwTask = call.Task;

	if (fd < 0 || fd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_close(&pwTask->Node[fd])){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
__closefrom(WIN_VNODE Nodes[], int offset)
{
	WIN_VNODE *pvNode = &Nodes[offset];

	while (offset < OPEN_MAX){
		if (pvNode->Access){
			vfs_close(pvNode);
		}
		offset++;
		pvNode++;
	}
	return(0);
}
int 
sys_closefrom(call_t call, int fd)
{
	int result = 0;
	WIN_TASK *pwTask = call.Task;

	if (fd < 0 || fd >= OPEN_MAX){
		result = -EBADF;
	}else{
		result = __closefrom(pwTask->Node, fd);
	}
	return(result);
}
ssize_t 
sys_read(call_t call, int fd, void *buf, size_t count)
{
	ssize_t result = 0;
	DWORD dwResult = -1;		/* WinNT EOF on ReadFile() */
	DWORD dwCount = count;
	WIN_TASK *pwTask = call.Task;

	if (fd < 0 || fd >= OPEN_MAX){
		return(-EBADF);
	}else if (!vfs_read(pwTask, &pwTask->Node[fd], buf, dwCount, &dwResult)){
		result -= errno_posix(GetLastError());
	}else if (dwResult == -1){
		pwTask->Error = errno_posix(GetLastError());
	}else{
		result = dwResult;
		if (pwTask->TracePoints & KTRFAC_GENIO){
			ktrace_GENIO(pwTask, fd, UIO_READ, buf, result);
		}
	}
	return(result);
}
ssize_t 
sys_write(call_t call, int fd, const void *buf, size_t nbytes)
{
	ssize_t result = 0;
	DWORD dwResult = 0;		/* WinNT EOF on WriteFile() */
	WIN_TASK *pwTask = call.Task;

	if (pwTask->TracePoints & KTRFAC_GENIO){
		ktrace_GENIO(pwTask, fd, UIO_WRITE, buf, nbytes);
	}
	if (fd < 0 || fd >= OPEN_MAX){
		return(-EBADF);
	}else if (!vfs_write(&pwTask->Node[fd], buf, nbytes, &dwResult)){
		result -= errno_posix(GetLastError());
	}else if (!dwResult){
		pwTask->Error = errno_posix(GetLastError());
	}else{
		result = dwResult;
	}
	return(result);
}
ssize_t 
sys_pread(call_t call, int fd, void *buf, size_t nbytes, off_t offset)
{
	ssize_t result = 0;
	DWORD dwResult = 0;		/* WinNT EOF on overlapped ReadFile() */
	WIN_TASK *pwTask = call.Task;

	if (offset < 0){
		result = -EINVAL;
	}else if (fd < 0 || fd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_pread(pwTask, &pwTask->Node[fd], buf, nbytes, offset, &dwResult)){
		result -= errno_posix(GetLastError());
	}else{
		result = dwResult;
	}
	return(result);
}
ssize_t 
sys_pwrite(call_t call, int fd, const void *buf, size_t nbytes, off_t offset)
{
	ssize_t result = 0;
	DWORD dwResult = -1;
	WIN_TASK *pwTask = call.Task;

	if (offset < 0){
		result = -EINVAL;
	}else if (fd < 0 || fd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_pwrite(&pwTask->Node[fd], buf, nbytes, offset, &dwResult)){
		result -= errno_posix(GetLastError());
	}else{
		result = dwResult;
	}
	return(result);
}

/****************************************************/

int 
sys_getthrid(call_t call)
{
	WIN_TASK *pwTask = call.Task;

	return(pwTask->TaskId);	/* libc.so raise() (git.exe) */
}
pid_t 
__getpgid(WIN_TASK *Task, pid_t pid)
{
	pid_t result = 0;

	if (pid < 0 || pid >= CHILD_MAX){
		result = -EINVAL;
	}else if (!pid){
		result = Task->GroupId;
	}else{
		result = __Tasks[pid].GroupId;
	}
	return(result);
}
pid_t 
sys_getpgid(call_t call, pid_t pid)
{
	return(__getpgid(call.Task, pid));
}
pid_t 
sys_getpgrp(call_t call)
{
	return(__getpgid(call.Task, 0));
}
int 
sys_setpgid(call_t call, pid_t pid, pid_t pgid)
{
	int result = 0;
	WIN_TASK *pwTask = call.Task;

	if (pid < 0 || pid >= CHILD_MAX){
		result = -EINVAL;
	}else{
		if (!pid){
			pid = pwTask->TaskId;
		}
		if (!pgid){
			pgid = __Tasks[pid].TaskId;
		}
		__Tasks[pid].GroupId = pgid;
	}
	return(result);
}
int 
sys_issetugid(call_t call)
{
	WIN_TASK *pwTask = call.Task;

	return(pwTask->IsSetUGid);
}
int 
sys_revoke(call_t call, const char *path)
{
	int result = 0;
	WIN_NAMEIDATA wPath = {0};
	WIN_FLAGS wFlags = {GENERIC_READ + GENERIC_WRITE, 
		FILE_SHARE_READ, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 0};
	WIN_MODE wMode = {0};
	WIN_VNODE vNode = {0};

	if (!vfs_open(path_win(&wPath, path, 0), &wFlags, mode_win(&wMode, 0700), &vNode)){
		result -= errno_posix(GetLastError());
	}else if (!vfs_revoke(&vNode)){
		result -= errno_posix(GetLastError());
	}else if (!vfs_close(&vNode)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_acct(call_t call, const char *path)
{
	WIN_ERR("acct(%s)\n", path);
	return(0);
}
int 
sys_profil(call_t call, char *samples, size_t size, u_long offset, u_int scale)
{
	int result = 0;

	if (scale && !samples){
		result = -EFAULT;
	}
	return(result);
}
int 
sys_obreak(call_t call, char *nsize)
{
	WIN_ERR("obreak(%d)\n", *nsize);
	return(0);
}
pid_t 
__tfork_thread(const struct __tfork *params, size_t psize, void (*startfunc)(void *), void *startarg)
{
	pid_t result = 0;
	DWORD dwResult = 0;

	if (!win___tfork_thread(NULL, psize, (PVOID)startfunc, startarg, &dwResult)){
		result -= errno_posix(GetLastError());
	}else{
		result = dwResult;
	}
	return(result);	
}
