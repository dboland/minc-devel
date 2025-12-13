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

#include <sys/stat.h>

#define SF_HIDDEN	0x00080000	/* from MacOS */
#define SF_SYSTEM	0x00100000
#define SF_DEVICE	0x00200000

static const mode_t __FTYPE_POSIX[] = {
	0,
	S_IFREG,
	S_IFDIR,
	S_IFBLK,
	S_IFCHR,
	S_IFLNK,
	S_IFSOCK,
	S_IFIFO,
	0
};

/****************************************************/

int 
rid_posix(SID8 *Sid)
{
	BYTE bAuth = Sid->IdentifierAuthority.Value[5];
	ULONG ulSubAuth = Sid->SubAuthority[0];
	ULONG ulRid = Sid->SubAuthority[Sid->SubAuthorityCount-1];
	int result = 0;

	if (!Sid->SubAuthorityCount){				/* Nt Pseudo/None */
		bAuth = SECURITY_NT_AUTHORITY_RID;
		ulSubAuth = SECURITY_NT_NON_UNIQUE;
		ulRid = DOMAIN_GROUP_RID_USERS;
	}else if (ulSubAuth == SECURITY_MACHINE_DOMAIN_RID){	/* Machine */
		if (!EqualPrefixSid(Sid, __SidMachine)){
			bAuth = SECURITY_OTHER_AUTHORITY_RID;
		}
	}else if (Sid->SubAuthorityCount == 1){
		ulSubAuth = 0;
	}else if (ulSubAuth == SECURITY_NT_SERVICE_RID){	/* NTService */
		bAuth = 0;
		ulSubAuth = 0;
	}
	result = bAuth * 100000000;		/* hello Andrew Tridgell */
	result += ulSubAuth * 1000000;
	result += ulRid;
	return(result);
}
SID8 *
rid_win(SID8 *Result, int rid)
{
	ULONG ulRid = rid % 1000000;
	BYTE bAuth = rid / 100000000;
	ULONG ulSubAuth = (rid / 1000000) % 100;

	if (rid == DOMAIN_NT_SERVICE_RID_INSTALLER){		/* TrustedInstaller */
		*Result = SidTrustedInstaller;
	}else if (rid < 0 || !bAuth){				/* tar.exe */
		CopySid(sizeof(SID8), Result, __SidNone);
	}else if (!ulSubAuth){					/* 0 (Local) */
		*Result = SidNull;
		Result->IdentifierAuthority.Value[5] = bAuth;
		Result->SubAuthority[0] = rid % 100000000;
	}else if (ulSubAuth == SECURITY_MACHINE_DOMAIN_RID){	/* 21 (Machine) */
		CopySid(sizeof(SID8), Result, __SidMachine);
		Result->SubAuthority[Result->SubAuthorityCount-1] = ulRid;
	}else if (ulSubAuth == SECURITY_BUILTIN_DOMAIN){	/* 32 (Builtin) */
		*Result = SidBuiltinAuth;
		Result->SubAuthority[1] = ulRid;
	}else{
		*Result = SidNull;
		Result->SubAuthorityCount++;
		Result->IdentifierAuthority.Value[5] = bAuth;
		Result->SubAuthority[0] = ulSubAuth;
		Result->SubAuthority[1] = ulRid;
	}
	return(Result);
}
time_t 
time_posix(const FILETIME *Time)
{
	DWORDLONG dwlTime = *(DWORDLONG *)Time;

	dwlTime -= 116444736000000000LL;	/* epoch (100-nanosecond intervals) */
	dwlTime *= 0.0000001;			/* seconds */
	return(dwlTime);
}
DWORD 
attr_win(unsigned int flags)
{
	DWORD dwResult = 0;

	if (flags & SF_ARCHIVED){
		dwResult |= FILE_ATTRIBUTE_ARCHIVE;
	}
	if (flags & SF_HIDDEN){
		dwResult |= FILE_ATTRIBUTE_HIDDEN;
	}
	if (flags & SF_IMMUTABLE){
		dwResult |= FILE_ATTRIBUTE_READONLY;
	}
	if (flags & SF_SYSTEM){
		dwResult |= FILE_ATTRIBUTE_SYSTEM;
	}
	if (flags & SF_DEVICE){
		dwResult |= FILE_ATTRIBUTE_DEVICE;
	}
	return(dwResult);
}
u_int32_t 
attr_posix(DWORD Attribs)
{
	u_int32_t result = 0;

	if (Attribs & FILE_ATTRIBUTE_ARCHIVE){
		result |= SF_ARCHIVED;
	}
	if (Attribs & FILE_ATTRIBUTE_HIDDEN){
		result |= SF_HIDDEN;
	}
	if (Attribs & FILE_ATTRIBUTE_READONLY){
		result |= SF_IMMUTABLE;
	}
	if (Attribs & FILE_ATTRIBUTE_SYSTEM){
		result |= SF_SYSTEM;
	}
	if (Attribs & FILE_ATTRIBUTE_DEVICE){
		result |= SF_DEVICE;
	}
	return(result);
}
mode_t 
mode_posix(WIN_MODE *Mode)
{
	mode_t result = 0;

	if (Mode->FileType < WIN_VMAX){
		result = __FTYPE_POSIX[Mode->FileType];
	}
	if (Mode->User & WIN_S_IREAD) result += S_IRUSR;
	if (Mode->User & WIN_S_IWRITE) result += S_IWUSR;
	if (Mode->User & WIN_S_IEXEC) result += S_IXUSR;
	if (Mode->Group & WIN_S_IREAD) result += S_IRGRP;
	if (Mode->Group & WIN_S_IWRITE) result += S_IWGRP;
	if (Mode->Group & WIN_S_IEXEC) result += S_IXGRP;
	if (Mode->Other & WIN_S_IREAD) result += S_IROTH;
	if (Mode->Other & WIN_S_IWRITE) result += S_IWOTH;
	if (Mode->Other & WIN_S_IEXEC) result += S_IXOTH;
	if (Mode->Special & WIN_S_ISUID) result += S_ISUID;
	if (Mode->Special & WIN_S_ISGID) result += S_ISGID;
	if (Mode->Special & WIN_S_ISTXT) result += S_ISTXT;
	return(result);
}
WIN_MODE *
mode_win(WIN_MODE *Result, mode_t mode)
{
	win_bzero(Result, sizeof(WIN_MODE));

	switch (mode & S_IFMT){
		case S_IFREG:
			Result->FileType = WIN_VREG;
			break;
		case S_IFDIR:
			Result->FileType = WIN_VDIR;
			break;
		case S_IFBLK:
			Result->FileType = WIN_VBLK;
			break;
		case S_IFCHR:
			Result->FileType = WIN_VCHR;
			break;
		case S_IFLNK:
			Result->FileType = WIN_VLNK;
			break;
		case S_IFSOCK:
			Result->FileType = WIN_VSOCK;
			break;
		case S_IFIFO:
			Result->FileType = WIN_VFIFO;
			break;
		default:
			Result->FileType = WIN_VNON;
	}
	if (mode & S_IRUSR) Result->User += WIN_S_IREAD;
	if (mode & S_IWUSR) Result->User += WIN_S_IWRITE;
	if (mode & S_IXUSR) Result->User += WIN_S_IEXEC;
	if (mode & S_IRGRP) Result->Group += WIN_S_IREAD;
	if (mode & S_IWGRP) Result->Group += WIN_S_IWRITE;
	if (mode & S_IXGRP) Result->Group += WIN_S_IEXEC;
	if (mode & S_IROTH) Result->Other += WIN_S_IREAD;
	if (mode & S_IWOTH) Result->Other += WIN_S_IWRITE;
	if (mode & S_IXOTH) Result->Other += WIN_S_IEXEC;
	if (mode & S_ISUID) Result->Special += WIN_S_ISUID;
	if (mode & S_ISGID) Result->Special += WIN_S_ISGID;
	if (mode & S_ISTXT) Result->Special += WIN_S_ISTXT;
	return(Result);
}
void 
stat_posix(WIN_TASK *Task, struct stat *buf, WIN_VATTR *Stat)
{
	win_bzero(buf, sizeof(struct stat));
	buf->st_dev = Stat->DeviceId;
	buf->st_ino = (Stat->FileIndexHigh * 0x100000000) + Stat->FileIndexLow;
	buf->st_mode = mode_posix(&Stat->Mode);
	buf->st_nlink = Stat->NumberOfLinks;
	buf->st_uid = rid_posix(&Stat->UserSid);
	buf->st_gid = rid_posix(&Stat->GroupSid);
	buf->st_size = (Stat->FileSizeHigh * 0x100000000) + Stat->FileSizeLow;
	buf->st_blocks = (buf->st_size + S_BLKSIZE - 1) / S_BLKSIZE;
	buf->st_blksize = WIN_S_BLKSIZE;
	/* raw device: device this inode represents (if special file) */
	buf->st_rdev = Stat->RawDeviceId;
	/* stat(2) man page: access (read, execve) */
	buf->st_atime = time_posix(&Stat->LastAccessTime);
	/* modification (write, futimes) */
	buf->st_mtime = time_posix(&Stat->LastWriteTime);
	/* status change (chown, chgrp, chmod, futimes) */
	buf->st_ctime = time_posix(&Stat->CreationTime);
	if (buf->st_uid == ROOT_UID){
		buf->st_uid = 0;
	}
	if (buf->st_gid == ROOT_GID){
		buf->st_gid = 0;
	}
	buf->st_flags = attr_posix(Stat->Attributes);
	if (Task->TracePoints & KTRFAC_STRUCT){
		ktrace_STRUCT(Task, "stat", 4, buf, sizeof(struct stat));
	}
}


/****************************************************/

int 
sys_fstat(call_t call, int fd, struct stat *buf)
{
	int result = 0;
	WIN_VATTR wStat = {0};
	WIN_TASK *pwTask = call.Task;

	if (fd < 0 || fd >= OPEN_MAX){
		return(-EBADF);
	}else if (!vfs_fstat(&pwTask->Node[fd], &wStat)){
		result -= errno_posix(GetLastError());
	}else{
		stat_posix(pwTask, buf, &wStat);
	}
	return(result);
}
int 
__fstatat(WIN_TASK *Task, int dirfd, const char *path, struct stat *buf, int flags)
{
	int result = 0;
	WIN_NAMEIDATA wPath = {0};
	WIN_VATTR wStat = {0};

	if (!path){
		result = -EFAULT;
	}else if (!path[0]){
		result = -ENOENT;
	}else if (!vfs_stat(pathat_win(&wPath, dirfd, path, flags), &wStat)){
		result -= errno_posix(GetLastError());
	}else{
		stat_posix(Task, buf, &wStat);
	}
	return(result);
}
int 
sys_lstat(call_t call, const char *path, struct stat *buf)
{
	int atflags = AT_SYMLINK | AT_INODE | AT_SYMLINK_NOFOLLOW;

	return(__fstatat(call.Task, AT_FDCWD, path, buf, atflags));
}
int 
sys_fstatat(call_t call, int dirfd, const char *path, struct stat *buf, int flag)
{
	return(__fstatat(call.Task, dirfd, path, buf, flag));
}
int 
sys_stat(call_t call, const char *path, struct stat *buf)
{
	return(__fstatat(call.Task, AT_FDCWD, path, buf, 0));
}

/****************************************************/

int 
__fchmodat(WIN_TASK *Task, int fd, const char *path, mode_t mode, int flag)
{
	int result = 0;
	int atflags = flag | AT_INODE | AT_LOCKLEAF;	/* syslogd.exe */
	WIN_MODE wMode;
	WIN_NAMEIDATA wPath = {0};

	if (!path){
		result = -EFAULT;
	}else if (!path[0]){
		result = -ENOENT;
	}else if (!vfs_chmod(pathat_win(&wPath, fd, path, atflags), mode_win(&wMode, mode))){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_fchmodat(call_t call, int fd, const char *path, mode_t mode, int flag)
{
	return(__fchmodat(call.Task, fd, path, mode, flag));
}
int 
sys_chmod(call_t call, const char *path, mode_t mode)
{
	return(__fchmodat(call.Task, AT_FDCWD, path, mode, 0));
}
int 
sys_fchmod(call_t call, int fd, mode_t mode)
{
	int result = 0;
	WIN_TASK *pwTask = call.Task;
	WIN_MODE wMode;

	if (fd < 0 || fd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_fchmod(&pwTask->Node[fd], &pwTask->UserSid, &pwTask->GroupSid, mode_win(&wMode, mode))){
		result -= errno_posix(GetLastError());
	}
	return(result);
}

/****************************************************/

int 
__chflagsat(int fd, const char *path, unsigned int flags, int atflag)
{
	int result = 0;
	WIN_NAMEIDATA wPath = {0};

	if (!disk_chflags(pathat_win(&wPath, fd, path, atflag), attr_win(flags))){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_chflags(call_t call, const char *path, unsigned int flags)
{
	return(__chflagsat(AT_FDCWD, path, flags, 0));
}
int 
sys_fchflags(int fd, unsigned int flags)
{
	// see: device/disk/disk_stat.c
	return(0);
}
mode_t 
sys_umask(call_t call, mode_t mask)
{
	WIN_TASK *pwTask = call.Task;
	mode_t result = pwTask->FileMask;

	pwTask->FileMask = mask;
	return(result);
}

/****************************************************/

int 
__mknodat(int fd, const char *path, mode_t mode, dev_t dev)
{
	int result = 0;
	WIN_NAMEIDATA wPath = {0};
	WIN_MODE wMode;

	if (!vfs_mknod(pathat_win(&wPath, fd, path, 0), mode_win(&wMode, mode), dev)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_mknodat(call_t call, int fd, const char *path, mode_t mode, dev_t dev)
{
	return(__mknodat(fd, path, mode, dev));
}
int 
sys_mknod(call_t call, const char *path, mode_t mode, dev_t dev)
{
	return(__mknodat(AT_FDCWD, path, mode, dev));
}
int 
__mkdirat(WIN_TASK *Task, int dirfd, const char *pathname, mode_t mode)
{
	int result = 0;
	WIN_MODE wMode;
	WIN_NAMEIDATA wPath = {0};

	mode &= (~Task->FileMask & 00777);
	if (!vfs_mkdir(pathat_win(&wPath, dirfd, pathname, 0), mode_win(&wMode, mode))){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_mkdirat(call_t call, int dirfd, const char *pathname, mode_t mode)
{
	return(__mkdirat(call.Task, dirfd, pathname, mode));
}
int 
sys_mkdir(call_t call, const char *pathname, mode_t mode)
{
	return(__mkdirat(call.Task, AT_FDCWD, pathname, mode));
}
int 
sys_mkfifoat(call_t call, int dirfd, const char *pathname, mode_t mode)
{
	int result = 0;
	WIN_MODE wMode;
	WIN_NAMEIDATA wPath = {0};
	WIN_VNODE vNode = {0};

	if (!vfs_mkfifo(pathat_win(&wPath, dirfd, pathname, 0), mode_win(&wMode, mode), &vNode)){
		result -= errno_posix(GetLastError());
	}else{
		result = fd_posix(call.Task, &vNode, 0);
	}
	return(result);
}
int 
sys_mkfifo(call_t call, const char *pathname, mode_t mode)
{
	return(sys_mkfifoat(call, AT_FDCWD, pathname, mode));
}
