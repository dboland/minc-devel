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

/* Flags in WIN_TASK struct */

#define WIN_PS_CONTROLT		0x00000001	/* Process has controlling terminal */
#define WIN_PS_EXEC		0x00000002	/* Process called exec. */
#define WIN_PS_INEXEC		0x00000004	/* Process is doing an exec right now */
#define WIN_PS_EXITING		0x00000008	/* Process is exiting */
#define WIN_PS_PPWAIT		0x00000040	/* Parent waits for exec/exit. */
#define WIN_PS_ISPWAIT		0x00000080	/* Is parent of PPWAIT child. */
#define WIN_PS_TRACED		0x00000200	/* Process is being traced */
#define WIN_PS_NOZOMBIE		0x00004000	/* pid 1 waits for me instead of dad */
#define WIN_PS_STOPPED		0x00008000	/* just stopped, need to send SIGCHLD */
#define WIN_PS_SYSTEM		0x00010000	/* No sigs, stats or swapping */
#define WIN_PS_EMBRYO		0x00020000	/* New process, not yet fledged */
#define WIN_PS_ZOMBIE		0x00040000	/* Dead and ready to be waited for */

#define DLL_PROCESS_DETACH      0
#define DLL_PROCESS_ATTACH      1
#define DLL_THREAD_ATTACH       2
#define DLL_THREAD_DETACH       3

/* sys/proc */

#define WIN_FORK_FORK		0x00000001
#define WIN_FORK_VFORK		0x00000002
#define WIN_FORK_IDLE		0x00000004
#define WIN_FORK_PPWAIT		0x00000008
#define WIN_FORK_SHAREFILES	0x00000010
#define WIN_FORK_SYSTEM		0x00000020
#define WIN_FORK_NOZOMBIE	0x00000040
#define WIN_FORK_SHAREVM	0x00000080
#define WIN_FORK_TFORK		0x00000100
#define WIN_FORK_SIGHAND	0x00000200
#define WIN_FORK_PTRACE		0x00000400
#define WIN_FORK_THREAD		0x00000800

/* sysctl() CTL_USER definitions */

#define CTL_USER	8

#define USER_PWD	1

#define PWD_UNSPEC		0
#define PWD_SETPWENT		1
#define PWD_ENDPWENT		2
#define PWD_GETPWENT		3
#define PWD_GETPWNAM		4
#define PWD_GETPWUID		5

#define USER_GRP	2

#define GRP_UNSPEC		0
#define GRP_SETGRENT		1
#define GRP_ENDGRENT		2
#define GRP_GETGRENT		3
#define GRP_GETGRNAM		4
#define GRP_GETGRGID		5
#define GRP_GETGROUPLIST	6

#define ROOT_UID	500000018
#define ROOT_GID	532000544
#define BIN_GID		532000545
#define DAEMON_UID	500000006
#define DAEMON_GID	500000019
#define OPERATOR_GID	532000556

#define USER_LOOKUP	3
#define USER_NAMEI	4

#define NAMEI_TTY		0
