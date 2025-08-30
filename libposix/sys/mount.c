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

#include <sys/mount.h>

/****************************************************/

void 
fsflags_debug(u_int32_t flags, const char *lable)
{
	DWORD dwFlags = flags;

	WIN_ERR("%s([0x%x]", lable, dwFlags);
	win_flagname(MNT_RDONLY, "RDONLY", dwFlags, &dwFlags);
	win_flagname(MNT_NOATIME, "NOATIME", dwFlags, &dwFlags);
	win_flagname(MNT_NOEXEC, "NOEXEC", dwFlags, &dwFlags);
	win_flagname(MNT_NOSUID, "NOSUID", dwFlags, &dwFlags);
	win_flagname(MNT_NODEV, "NODEV", dwFlags, &dwFlags);
	win_flagname(MNT_SYNCHRONOUS, "SYNCHRONOUS", dwFlags, &dwFlags);
	win_flagname(MNT_ASYNC, "ASYNC", dwFlags, &dwFlags);
	win_flagname(MNT_SOFTDEP, "SOFTDEP", dwFlags, &dwFlags);
	win_flagname(MNT_UPDATE, "UPDATE", dwFlags, &dwFlags);
	win_flagname(MNT_ROOTFS, "ROOTFS", dwFlags, &dwFlags);
	WIN_ERR("[0x%x])\n", dwFlags);
}
char *
fstype_posix(char *type, void *options, LPWSTR TypeName)
{
	if (!wcsncmp(TypeName, L"FAT", 3)){
		struct msdosfs_args *msdos = options;
		win_strlcpy(type, "msdos", MFSNAMELEN);
		msdos->uid = rid_posix(&SidEveryone);
		msdos->gid = rid_posix(&SidEveryone);
		msdos->mask = 00777;
	}else if (!wcscmp(TypeName, L"ISO9660")){
		win_strlcpy(type, "cd9660", MFSNAMELEN);
	}else{
		win_wcstombs(type, win_wcslcase(TypeName), MFSNAMELEN);
	}
	return(type);
}
u_int32_t 
fsflags_posix(LARGE_INTEGER *Flags)
{
	u_int32_t result = Flags->HighPart;
	DWORD dwFlags = Flags->LowPart;

	if (dwFlags & FILE_READ_ONLY_VOLUME){
		result |= MNT_RDONLY;
	}
	if (!(dwFlags & FILE_PERSISTENT_ACLS)){
		result |= MNT_NOSUID;
	}
	if (dwFlags & FILE_VOLUME_QUOTAS){
		result |= MNT_QUOTA;
	}
	return(result);
}
DWORD 
fsflags_win(u_int32_t flags)
{
	DWORD dwResult = 0;

//fsflags_debug(flags, "fsflags_win");
	if (flags & MNT_RDONLY){
		dwResult |= FILE_READ_ONLY_VOLUME;
	}
	if (!(flags & MNT_NOSUID)){
		dwResult |= FILE_PERSISTENT_ACLS;
	}
	if (flags & MNT_QUOTA){
		dwResult |= FILE_VOLUME_QUOTAS;
	}
	return(dwResult);
}
struct statfs *
statfs_posix(struct statfs *buf, WIN_STATFS *Stat)
{
	WIN_DEVICE *pwDevice = DEVICE(Stat->DeviceId);

	win_bzero(buf, sizeof(struct statfs));

	buf->f_flags = fsflags_posix(&Stat->Flags);
	buf->f_namemax = Stat->MaxPath;
	buf->f_bsize = Stat->BytesPerSector * Stat->SectorsPerCluster;
	buf->f_blocks = Stat->ClustersTotal;
	buf->f_bfree = Stat->FreeClusters;
	buf->f_bavail = Stat->FreeClusters;

	buf->f_fsid.val[0] = pwDevice->DeviceId >> 8;
	buf->f_fsid.val[1] = pwDevice->DeviceId & 0xFF;
	buf->f_ctime = time_posix(&Stat->MountTime);

	/* fs type name and options */
	fstype_posix(buf->f_fstypename, &buf->mount_info, Stat->TypeName);

	/* directory on which mounted */
	pathn_posix(buf->f_mntonname, win_wcslcase(Stat->Path), MNAMELEN);

	/* mounted file system */
	win_strlcpy(win_stpcpy(buf->f_mntfromname, "/dev/"), pwDevice->Name, MNAMELEN - 5);

	/* special for mount request */
	win_strlcpy(buf->f_mntfromspec, buf->f_mntfromname, MNAMELEN);

	return(buf + 1);
}

/****************************************************/

int
mount_NTFS(WIN_NAMEIDATA *Path, DWORD Flags, struct ntfs_args *args)
{
	int result = 0;
	WIN_NAMEIDATA wdPath = {0};
	WIN_MODE wMode = {0};
	WIN_FLAGS wFlags = {0};
	WIN_VNODE vNode;

	if (!vfs_open(path_win(&wdPath, args->fspec, O_NOFOLLOW), &wFlags, &wMode, &vNode)){
		result -= errno_posix(GetLastError());
	}else if (!vfs_mount(&vNode, Path, Flags, mode_win(&wMode, args->mode))){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int
mount_MSDOS(WIN_NAMEIDATA *Path, DWORD Flags, struct msdosfs_args *args)
{
	int result = 0;
	WIN_NAMEIDATA wdPath = {0};
	WIN_MODE wMode = {0};
	WIN_FLAGS wFlags = {0};
	WIN_VNODE vNode;

	if (!vfs_open(path_win(&wdPath, args->fspec, O_NOFOLLOW), &wFlags, &wMode, &vNode)){
		result -= errno_posix(GetLastError());
	}else if (!vfs_mount(&vNode, Path, Flags, mode_win(&wMode, args->mask))){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int
mount_CD9660(WIN_NAMEIDATA *Path, DWORD Flags, struct iso_args *args)
{
	int result = 0;
	WIN_NAMEIDATA wdPath = {0};
	WIN_MODE wMode = {0};
	WIN_FLAGS wFlags = {0};
	WIN_VNODE vNode;

	if (!vfs_open(path_win(&wdPath, args->fspec, O_NOFOLLOW), &wFlags, &wMode, &vNode)){
		result -= errno_posix(GetLastError());
	}else if (!vfs_mount(&vNode, Path, Flags, mode_win(&wMode, 0666))){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int
mount_UFS(WIN_NAMEIDATA *Path, DWORD Flags, struct ufs_args *args)
{
	int result = 0;
	WIN_NAMEIDATA wdPath = {0};
	WIN_MODE wMode = {0};
	WIN_FLAGS wFlags = {0};
	WIN_VNODE vNode;

	if (!vfs_open(path_win(&wdPath, args->fspec, O_NOFOLLOW), &wFlags, &wMode, &vNode)){
		result -= errno_posix(GetLastError());
	}else if (!vfs_mount(&vNode, Path, Flags, mode_win(&wMode, 0666))){
		result -= errno_posix(GetLastError());
	}
	return(result);
}

/****************************************************/

int 
sys_mount(call_t call, const char *type, const char *dir, int flags, void *data)
{
	int result = 0;
	WIN_NAMEIDATA wPath = {0};
	DWORD dwFlags = fsflags_win(flags);

	/* mount(2)
	 */
	if (!strcmp(type, MOUNT_NTFS)){
		result = mount_NTFS(path_win(&wPath, dir, O_NOCROSS), dwFlags, (struct ntfs_args *)data);

	}else if (!strcmp(type, "refs")){
		result = mount_NTFS(path_win(&wPath, dir, O_NOCROSS), dwFlags, (struct ntfs_args *)data);

	}else if (!strcmp(type, MOUNT_MSDOS)){
		result = mount_MSDOS(path_win(&wPath, dir, O_NOCROSS), dwFlags, (struct msdosfs_args *)data);

	}else if (!strcmp(type, MOUNT_CD9660)){
		result = mount_CD9660(path_win(&wPath, dir, O_NOCROSS), dwFlags, (struct iso_args *)data);

	}else if (!strcmp(type, MOUNT_UFS)){
		result = mount_UFS(path_win(&wPath, dir, O_NOCROSS), dwFlags, (struct ufs_args *)data);

	}else{
		result = -EOPNOTSUPP;

	}
	return(result);
}
int
sys_unmount(call_t call, const char *dir, int flags)
{
	int result = 0;
	WIN_NAMEIDATA wPath = {0};

	if (!vfs_unmount(path_win(&wPath, dir, O_NOCROSS))){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_statfs(call_t call, const char *path, struct statfs *buf)
{
	int result = 0;
	WIN_STATFS fsInfo = {0};
	WIN_NAMEIDATA wPath = {0};
	WIN_TASK *pwTask = call.Task;

	if (!vfs_statfs(path_win(&wPath, path, 0), &fsInfo)){
		result -= errno_posix(GetLastError());
	}else{
		statfs_posix(buf, &fsInfo);
	}
	return(result);
}
int 
sys_fstatfs(call_t call, int fd, struct statfs *buf)
{
	int result = 0;
	WIN_STATFS fsInfo = {0};
	WIN_NAMEIDATA wPath = {0};
	WIN_TASK *pwTask = call.Task;

	if (!vfs_statfs(pathat_win(&wPath, fd, "", AT_SYMLINK_FOLLOW), &fsInfo)){
		result -= errno_posix(GetLastError());
	}else{
		statfs_posix(buf, &fsInfo);
	}
	return(result);
}
int 
sys_getfsstat(call_t call, struct statfs *buf, size_t bufsize, int flags)
{
	int result = 0;
	WIN_STATFS fsInfo;
	DWORD dwFlags = flags;
	DWORD dwIndex = 0;
	WIN_MOUNT *pwMount = __Mounts;

	while (dwIndex < WIN_MOUNT_MAX){
		if (pwMount->Flags.QuadPart){
			if (buf){
				drive_statfs(pwMount, &fsInfo);
				buf = statfs_posix(buf, &fsInfo);
			}
			result++;
		}
		dwIndex++;
		pwMount++;
	}
	return(result);
}
