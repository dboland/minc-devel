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

#include "dev_types.h"
#include "vfs_types.h"

BOOL vfs_PROCESS_ATTACH(HINSTANCE Instance, LPVOID Reserved);
BOOL vfs_PROCESS_DETACH(WIN_TASK *Task);
BOOL vfs_THREAD_DETACH(WIN_TASK *Task);
BOOL vfs_start(DWORD FileSystem);
BOOL vfs_finish(DWORD FileSystem);

/* vfs_unistd.c */

BOOL vfs_access(WIN_NAMEIDATA *Path, ACCESS_MASK Access);
BOOL vfs_rename(WIN_NAMEIDATA *Path, WIN_NAMEIDATA *NewPath);
BOOL vfs_renameat(WIN_VNODE *Node, WIN_NAMEIDATA *Result);
BOOL vfs_close(WIN_VNODE *Node);
BOOL vfs_dup(WIN_VNODE *Node, WIN_VNODE *Result);
BOOL vfs_dup2(WIN_VNODE *Node, WIN_VNODE *Result);
BOOL vfs_ftruncate(WIN_VNODE *Node, LARGE_INTEGER *Size);
BOOL vfs_truncate(WIN_NAMEIDATA *Path, LARGE_INTEGER *Size);
BOOL vfs_unlink(WIN_NAMEIDATA *Path);
BOOL vfs_lseek(WIN_VNODE *Node, LARGE_INTEGER *Offset, DWORD Method, LARGE_INTEGER *Result);
BOOL vfs_rmdir(WIN_NAMEIDATA *Path);
BOOL vfs_link(WIN_NAMEIDATA *Path, WIN_NAMEIDATA *Result);
BOOL vfs_fsync(WIN_VNODE *Node);
BOOL vfs_chdir(WIN_TASK *Task, WIN_NAMEIDATA *Path);
BOOL vfs_sync(VOID);
BOOL vfs_chroot(WIN_NAMEIDATA *Path);
BOOL vfs_revoke(WIN_VNODE *Node);
BOOL vfs_read(WIN_TASK *Task, WIN_VNODE *Node, LPVOID Buffer, DWORD Size, DWORD *Result);
BOOL vfs_write(WIN_VNODE *Node, LPCVOID Buffer, DWORD Size, DWORD *Result);
BOOL vfs_pwrite(WIN_VNODE *Node, LPCVOID Buffer, DWORD Size, DWORDLONG Offset, DWORD *Result);
BOOL vfs_pread(WIN_TASK *Task, WIN_VNODE *Node, LPVOID Buffer, DWORD Size, DWORDLONG Offset, DWORD *Result);
VOID vfs_closefrom(WIN_VNODE Nodes[]);
BOOL vfs_readlink(WIN_NAMEIDATA *Path);
BOOL vfs_symlink(WIN_NAMEIDATA *Path, WIN_MODE *Mode, WIN_NAMEIDATA *Target);
BOOL vfs_chown(WIN_NAMEIDATA *Path, PSID NewUser, PSID NewGroup);
BOOL vfs_fchown(WIN_TASK *Task, WIN_VNODE *Node, PSID NewUser, PSID NewGroup);

/* vfs_syscall.c */

BOOL vfs_seteuid(WIN_TASK *Task, SID8 *Sid);
BOOL vfs_setegid(WIN_TASK *Task, SID8 *Sid);
BOOL vfs_setlogin(WIN_TASK *Task, LPCSTR Name);
BOOL vfs_getlogin(WIN_TASK *Task, LPSTR Name, DWORD Size);
BOOL vfs_pipe(WIN_VNODE Result[2]);
BOOL vfs_setugid(WIN_TASK *Task);

/* vfs_termios.c */

BOOL vfs_TIOCGWINSZ(WIN_VNODE *Node, WIN_WINSIZE *WinSize);
BOOL vfs_TIOCSWINSZ(WIN_VNODE *Node, WIN_WINSIZE *WinSize);
BOOL vfs_TIOCFLUSH(WIN_VNODE *Node);
BOOL vfs_TIOCDRAIN(WIN_VNODE *Node);
BOOL vfs_TIOCGETA(WIN_VNODE *Node, WIN_TERMIO *Mode);
BOOL vfs_TIOCSETA(WIN_VNODE *Node, WIN_TERMIO *Mode, BOOL Flush, BOOL Drain);
BOOL vfs_PTMGET(WIN_VNODE *Node, WIN_PTMGET *Result);
BOOL vfs_TIOCSCTTY(WIN_VNODE *Node, WIN_TASK *Task);
BOOL vfs_TIOCGPGRP(WIN_VNODE *Node, UINT *Result);
BOOL vfs_TIOCSPGRP(WIN_VNODE *Node, UINT GroupId);
BOOL vfs_TIOCGFLAGS(WIN_TTY *Terminal, UINT *Result);
BOOL vfs_TIOCSFLAGS(WIN_TTY *Terminal, UINT Flags);

/* vfs_stat.c */

BOOL vfs_fstat(WIN_VNODE *Node, WIN_VATTR *Result);
BOOL vfs_stat(WIN_NAMEIDATA *Path, WIN_VATTR *Result);
BOOL vfs_chmod(WIN_NAMEIDATA *Path, WIN_MODE *Mode);
BOOL vfs_mkfifo(WIN_NAMEIDATA *Path, WIN_MODE *Mode, WIN_VNODE *Result);
BOOL vfs_mknod(WIN_NAMEIDATA *Path, WIN_MODE *Mode, DWORD DeviceId);
BOOL vfs_fchmod(WIN_VNODE *Node, SID8 *Owner, SID8 *Group, WIN_MODE *Mode);
BOOL vfs_mkdir(WIN_NAMEIDATA *Path, WIN_MODE *Mode);

/* vfs_signal.c */

BOOL vfs_sigsuspend(WIN_TASK *Task, CONST UINT *Mask);
BOOL vfs_raise(UINT Message, WPARAM WParam, LPARAM LParam);
BOOL vfs_sigaction(BOOL Enable);
BOOL vfs_kill_PID(DWORD ThreadId, UINT Message, WPARAM WParam, LPARAM LParam);
BOOL vfs_kill_GRP(DWORD GroupId, UINT Message, WPARAM WParam, LPARAM LParam);
BOOL vfs_kill_ANY(DWORD ParentId, UINT Message, WPARAM WParam, LPARAM LParam);
BOOL vfs_kill_SYS(DWORD CallerId, UINT Message, WPARAM WParam, LPARAM LParam);

/* vfs_fcntl.c */

BOOL vfs_open(WIN_NAMEIDATA *Path, WIN_FLAGS *Flags, WIN_MODE *Mode, WIN_VNODE *Result);
BOOL vfs_F_SETLK(WIN_VNODE *Node, DWORD Flags, LARGE_INTEGER *Offset, LARGE_INTEGER *Size);
BOOL vfs_F_SETFL(WIN_VNODE *Node, WIN_FLAGS *Flags);
BOOL vfs_F_GETPATH(WIN_VNODE *Node, SID8 *Owner, SID8 *Group, WIN_NAMEIDATA *Result);
BOOL vfs_F_DUPFD(WIN_VNODE *Node, BOOL CloseExec, WIN_VNODE *Result);
HANDLE vfs_F_OSFHANDLE(WIN_VNODE Nodes[], DWORD Index);

/* vfs_mount.c */

BOOL vfs_statfs(WIN_NAMEIDATA *Path, WIN_STATFS *Result);
BOOL vfs_mount(WIN_VNODE *Node, WIN_NAMEIDATA *Path, DWORD Flags, WIN_MODE *Mode);
BOOL vfs_getfsstat(WIN_CFDATA *Config, WIN_CFDRIVER *Driver, WIN_STATFS *Result);
BOOL vfs_unmount(WIN_NAMEIDATA *Path);

/* vfs_statvfs.c */

BOOL vfs_setvfs(WIN_CFDATA *Config, DWORD Flags);
VOID vfs_endvfs(WIN_CFDATA *Config);
BOOL vfs_getvfs(WIN_CFDATA *Config, DWORD Flags);

/* vfs_time.c */

BOOL vfs_setitimer(WIN_TASK *Task, LONG *Interval, DWORDLONG *TimeOut);
BOOL vfs_nanosleep(WIN_TASK *Task, DWORDLONG TimeOut, DWORDLONG *Remain);
BOOL vfs_utimes(WIN_NAMEIDATA *Path, FILETIME FileTime[2]);

/* vfs_stdlib.c */

BOOL vfs_atexit(WIN_ATEXITPROC Function);
BOOL vfs__cxa_finalize(PVOID Param);

/* vfs_ktrace.c */

BOOL vfs_ktrace_SET(WIN_TASK *Task, WIN_NAMEIDATA *Path, PVOID Buffer, LONG Size);
BOOL vfs_ktrace_CLEAR(WIN_TASK *Task);

/* vfs_filio.c */

BOOL vfs_FIONREAD(WIN_VNODE *Node, ULONG *Result);
BOOL vfs_FIONBIO(WIN_VNODE *Node, ULONG NoWait);

/* vfs_namei.c */

WIN_NAMEIDATA *vfs_lookup(WIN_NAMEIDATA *Path, LPWSTR Source, DWORD Flags);
BOOL vfs_namei(HANDLE Handle, DWORD Index, WIN_VNODE *Result);

/* vfs_uio.c */

BOOL vfs_writev(WIN_VNODE *Node, const WIN_IOVEC Data[], DWORD Count, DWORD *Result);
BOOL vfs_readv(WIN_TASK *Task, WIN_VNODE *Node, const WIN_IOVEC Data[], LONG Count, ULONG *Result);

/* vfs_resource.c */

BOOL vfs_getrusage_CHILDREN(DWORD ParentId, WIN_RUSAGE *Result);

/* vfs_reboot.c */

BOOL vfs_reboot(WIN_TASK *Task, USHORT Type, USHORT DeviceId);

/* vfs_libgen.c */

VOID vfs_setproctitle(LPCSTR Title);
LPWSTR vfs_dirname(LPWSTR Result, LPCWSTR FileName);

/* vfs_dirent.c */

BOOL vfs_getdents(WIN_NAMEIDATA *Path, PVOID Buffer, DWORD Size, DWORD *Result);

/* vfs_poll.c */

BOOL vfs_poll(WIN_TASK *Task, WIN_VNODE *Nodes[], WIN_POLLFD *Info[], DWORD *TimeOut, DWORD *Result);

/* vfs_wait.c */

BOOL vfs_wait4(WIN_TASK *Task, WIN_TASK *Children[], BOOL NoHang, DWORD Status, WIN_RUSAGE *Result);

/* vfs_systm.c */

VOID vfs_consinit(VOID);

/****************************************************/

/* process.c */

WIN_TASK *proc_init(WIN_SIGPROC SignalProc);
WIN_TASK *proc_dup(WIN_TASK *Parent, WIN_THREAD_STRUCT *Thread);
BOOL proc_close(WIN_TASK *Task);
BOOL proc_poll(WIN_TASK *Task);
DECLSPEC_NORETURN VOID proc_exit(DWORD ExitCode);
BOOL proc_execve(WIN_TASK *Task, LPSTR Command, PVOID Environ);
BOOL proc_orphanize(WIN_TASK *Task);
PVOID proc_malloc(WIN_TASK *Task, ULONG Size);
BOOL proc_realloc(WIN_TASK *Task, ULONG Size, PVOID Buffer, PVOID *Result);
VOID proc_free(WIN_TASK *Task, PVOID Buffer);
BOOL proc_setsid(WIN_TASK *Task);

/* drive.c */

BOOL drive_statvfs(WIN_CFDATA *Config, DWORD Flags, WIN_CFDRIVER *Result);
BOOL drive_match(LPCWSTR NtName, DWORD DeviceType, WIN_CFDRIVER *Driver);
BOOL drive_statfs(WIN_MOUNT *Mount, WIN_STATFS *Result);

/* disk.c */

BOOL disk_chflags(WIN_NAMEIDATA *Path, DWORD Attributes);
BOOL disk_futimes(WIN_VNODE *Node, FILETIME FileTime[2]);
BOOL disk_HW_DISKNAMES(WIN_DEVICE *Device, LPSTR Result);

/* registry.c */

WIN_NAMEIDATA *reg_lookup(WIN_NAMEIDATA *Path, LPCWSTR Source, DWORD Flags);
BOOL reg_open(WIN_NAMEIDATA *Path, WIN_FLAGS *Flags, WIN_VNODE *Result);
BOOL reg_read(WIN_VNODE *Node, LPCWSTR Name, LPVOID Buffer, DWORD Size, DWORD *Result);
BOOL reg_close(WIN_VNODE *Node);

/* pdo.c */

DWORD pdo_statvfs(WIN_CFDATA *Config, DWORD Flags, WIN_CFDRIVER *Result);
BOOL pdo_match(LPCWSTR NtName, DWORD DeviceType, WIN_CFDRIVER *Driver);
BOOL pdo_DIOCGDINFO(WIN_DEVICE *Device);
BOOL pdo_WSKBDIO_GTYPE(UINT *Type, UINT *SubType, UINT *FKeys);
BOOL rand_read(LPSTR Buffer, DWORD Size, DWORD *Result);

/* char.c */

BOOL con_revoke(WIN_TTY *Terminal, WIN_DEVICE *Device);

/* volume.c */

BOOL vol_fstat(HANDLE Handle, LPSTR Result);
BOOL vol_stat(LPCWSTR Path, LPSTR Result);

/* winsock.c */

UINT ws2_nametoindex(LPCSTR Name);
UINT ws2_indextoname(DWORD Index, LPSTR Result);
