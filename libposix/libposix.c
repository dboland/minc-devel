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

#include <stdarg.h>
#include <stdint.h>
#include <wchar.h>

#include <sys/sysctl.h>
#include <sys/sched.h>

#include "win/windows.h"		/* minimal Win32 */
#include "win/winsock2.h"		/* minimal Windows Sockets v.2 */
#include "win/iphlpapi.h"		/* minimal Network API */
#include "ntdll_posix.h"
#include "win_posix.h"
#include "msvc_posix.h"
#include "vfs_posix.h"
#include "ws2_posix.h"
#include "bsd_posix.h"

#include "../libtrace/libtrace.h"

extern SID8 SidNull;
extern SID8 SidBuiltinAuth;
extern SID8 SidAdmins;
extern SID8 SidEveryone;
extern SID8 SidSystem;
extern SID8 SidUsers;
extern SID8 SidTrustedInstaller;

extern WIN_TASK 	*__Tasks;
extern WIN_DEV_CLASS 	*__Devices;
extern WIN_TTY 		*__Terminals;
extern WIN_PSTRING 	*__Strings;
extern WIN_MOUNT	*__Mounts;
extern WIN_GLOBALS	*__Globals;
extern WIN_TTY	 	*__CTTY;
extern SID8 		*__SidNone;
extern SID8 		*__SidMachine;

extern DWORD	 	__TlsIndex;
extern HANDLE		__Interrupt;
extern HANDLE		__PipeEvent;

u_long	__THREAD_FRAME;

CHAR 			*__PROGNAME;		/* ktrace.c */

#define WIN_ERR		msvc_printf

/* hello Terry Davis */

#include "kern/syscall.h"
#include "kern/syscalls.c"
#include "string.c"
#include "sys/errno.c"
#include "sys/uio.c"
#include "sys/ktrace.c"
#include "sys/vnode.c"
#include "sys/namei.c"
#include "sys/msgbuf.c"
#include "sys/systm.c"		/* cpu_configure() */
#include "sys/stat.c"
#include "sys/syscall.c"
#include "sys/fcntl.c"
#include "sys/time.c"
#include "sys/proc.c"
#include "netinet/in.c"
#include "netinet6/in6.c"
#include "sys/socket.c"
#include "sys/tty.c"
#include "sys/termio.c"
#include "sys/filio.c"
#include "sys/dkio.c"
#include "sys/mtio.c"
#include "sys/pciio.c"
#include "sys/resource.c"
#include "sys/unistd.c"
#include "sys/wait.c"
#include "sys/signal.c"
#include "sys/task.c"
#include "sys/mman.c"
#include "sys/poll.c"
#include "sys/select.c"
#include "sys/mount.c"
#include "sys/sem.c"
#include "sys/swap.c"
#include "sys/reboot.c"
#include "sys/dirent.c"
#include "dev/wscons/wsconsio.c"
#include "net/if.c"		/* ifinit() */
#include "net/if_dl.c"
#include "net/if_var.c"
#include "netinet/ip.c"
#include "netinet6/ip6.c"
#include "net/route.c"
#include "netinet/in_pcb.c"
#include "netinet/tcp.c"
#include "netinet/udp.c"
#include "compat.c"
#include "utime.c"
#include "pwd.c"
#include "grp.c"
#include "sys/sockio.c"
#include "sys/ioctl.c"
#include "sys/msg.c"
#include "sys/file.c"
#include "sys/kernel.c"
#include "sys/sysctl.c"
#include "kern/init_sysent.c"

/****************************************************/

BOOL 
posix_THREAD_ATTACH(WIN_TASK *Task)
{
	return(TRUE);
}
BOOL 
posix_PROCESS_DETACH(WIN_TASK *Task)
{
	if (Task->Flags & WIN_PS_EXITING){		/* ktrace.exe */
		Task->Flags |= WIN_PS_ZOMBIE;
		Task->State = WIN_SZOMB;
		vfs_closefrom(Task->Node);
		if (!vfs_kill_PID(pid_win(Task->ParentId), WM_COMMAND, CTRL_CHILD_EVENT, Task->TaskId)){
			proc_orphanize(Task);
		}
	}
	return(TRUE);
}
BOOL 
posix_THREAD_DETACH(WIN_TASK *Task)
{
	if (Task->Flags & WIN_PS_PPWAIT){
		win_kill(pid_win(Task->ParentId), WM_USER, Task->TaskId, 0);
	}
	if (Task->Flags & WIN_PS_EXITING){
		Task->Flags |= WIN_PS_ZOMBIE;
		Task->State = WIN_SZOMB;
		__closefrom(Task->Node, 0);
		if (!vfs_kill_PID(pid_win(Task->ParentId), WM_COMMAND, CTRL_CHILD_EVENT, Task->TaskId)){
			proc_orphanize(Task);
		}
	}
	return(TRUE);
}
