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

#include "arch_types.h"
#include "shell_types.h"

/* winerror.h */

#define ERROR_BAD_FILE_TYPE		222L		/* Invalid file system */
#define ERROR_ELEVATION_REQUIRED	740L		/* The requested operation requires elevation */
#define ERROR_SQL_ACCESS_DENIED		15105L		/* Vista panic: no access to error strings (de-elevating) */

/* winbase.h */

#define MAX_LABEL			32
#define MAX_NAME			64		/* ISO 9660 (Joliet) */
#define MAX_GUID			96
#define MAX_COMMENT			160
#define MAX_MESSAGE			256
#define MAX_TEXT			416
#define MAX_ARGBUF			0x07FFF		/* length of UNICODE_STRING struct */
//#define MAX_ARGBUF			0x40000		/* GNU configure */
#define MAX_FILESIZE			0xFFFFFFF0000	/* 16 TB minus 64 KB */

/* winnt.h */

/* We need at least FILE_WRITE_ATTRIBUTES to CreateHardLink() 
 * on a file with zero permissions (mutt_dotlock.exe).
 */

#define WIN_S_IREAD	(FILE_READ_DATA)
#define WIN_S_IWRITE	(FILE_WRITE_DATA | FILE_WRITE_EA | FILE_APPEND_DATA)
#define WIN_S_IEXEC	(FILE_EXECUTE)
#define WIN_S_IRX	(WIN_S_IREAD | WIN_S_IEXEC)
#define WIN_S_IRW	(WIN_S_IREAD | WIN_S_IWRITE)
#define WIN_S_IRWX	(WIN_S_IRW | WIN_S_IEXEC)

#define WIN_S_BLKSIZE	4096

#define SYSTEM_TIMEOFDAY_INFORMATION	SYSTEM_TIME_OF_DAY_INFORMATION

/* from Cygwin */

#define WIN_S_ISTXT	FILE_READ_DATA
#define WIN_S_ISGID	FILE_WRITE_DATA
#define WIN_S_ISUID	FILE_APPEND_DATA

/* winternl.h */

typedef struct _SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION {
	LARGE_INTEGER IdleTime;
	LARGE_INTEGER KernelTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER Reserved[2];
	ULONG InterruptCount;
} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;

/*
 * win_acl.c
 */

#define ALL_INHERIT_ACE			(OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE)

#define WIN_ACL_TYPE_ACCESS		0x8000
#define WIN_ACL_TYPE_DEFAULT		0x4000

#define WIN_ACL_USER_OBJ		0x01
#define WIN_ACL_USER			0x02
#define WIN_ACL_GROUP_OBJ		0x04
#define WIN_ACL_GROUP			0x08
#define WIN_ACL_MASK			0x10	/* resolved Unix permissions? */
#define WIN_ACL_OTHER			0x20

/* Authority Identifiers */

#define SECURITY_NT_AUTHORITY_RID		5
#define SECURITY_OTHER_AUTHORITY_RID		6

/* Subauthority Identifiers */

#define SECURITY_NT_NON_UNIQUE_RID		0x15
#define SECURITY_MACHINE_DOMAIN_RID		SECURITY_NT_NON_UNIQUE_RID

/* Vista */

#define SECURITY_NT_SERVICE_RID			80
#define DOMAIN_NT_SERVICE_RID_INSTALLER		0x876402C0	/* TrustedInstaller */

typedef struct _WIN_MODE {
	DWORD FileType;
	DWORD User;
	DWORD Group;
	DWORD Other;
	DWORD Special;
} WIN_MODE;

typedef struct _SID8 {
	BYTE Revision;
	BYTE SubAuthorityCount;
	SID_IDENTIFIER_AUTHORITY IdentifierAuthority;
	ULONG SubAuthority[8];
} SID8, *PSID8;

typedef struct _ACCESS_ALLOWED_ACE8 {
	ACE_HEADER Header;
  	ACCESS_MASK Mask;
	SID8 Sid;
} ACCESS_ALLOWED_ACE8;

typedef struct _ACE_PEEK {
	ACE_HEADER Header;
  	ACCESS_MASK Mask;
	SID Sid;
} ACE_PEEK;

typedef struct _WIN_OBJECT_CONTROL {
	SECURITY_DESCRIPTOR Security;
	SID8 SidUser;
	ACL DefaultAcl;
	ACCESS_ALLOWED_ACE8 Ace[3];
} WIN_OBJECT_CONTROL;

#define ACL_ACESIZE			(sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD))

/*
 * win_sysctl.c
 */

typedef struct _WIN_KUSAGE {
	FILETIME Creation;
	FILETIME Exit;
	FILETIME Kernel;
	FILETIME User;
} WIN_KUSAGE;

/*
 * win_capability.c
 */

#define TOKEN_ADJUST_SESSIONID		0x100

typedef struct _WIN_CAP_CONTROL {
	HANDLE Token;
	LUID AuthId;
	SID8 User;
	SID8 Primary;
	DWORD SessionId;
	PTOKEN_GROUPS Groups;
	PTOKEN_PRIVILEGES Privs;
	ACL DefaultACL;
	ACCESS_ALLOWED_ACE8 Ace[4];
} WIN_CAP_CONTROL;

/*
 * win_stdlib.c
 */

//#define MIN_BUFSIZE		1024

#define WIN_MSGBUFSIZE		(4 * WIN_PAGE_SIZE)
#define WIN_ATEXIT_MAX		32

typedef VOID (*WIN_ATEXITPROC)(VOID);

/*
 * win_sched.c
 */

/* Init, svinit, sh, init.sh, systemd, upstart, openrc all can act as
 * init programs. (Rob Landley, OpenioTSummit, 2017, 1:24:30)
 */

#define WIN_PID_INIT	1
#define WIN_CHILD_MAX	80

#define WIN_P_IREAD	(GENERIC_READ | PROCESS_TERMINATE)
#define WIN_P_IWRITE	(STANDARD_RIGHTS_REQUIRED | PROCESS_SET_SESSIONID | PROCESS_SET_INFORMATION | PROCESS_VM_WRITE | PROCESS_SET_QUOTA)
#define WIN_P_IEXEC	(GENERIC_EXECUTE | PROCESS_DUP_HANDLE | PROCESS_VM_OPERATION | PROCESS_SUSPEND_RESUME | PROCESS_CREATE_THREAD | PROCESS_CREATE_PROCESS)
#define WIN_P_IRWX	(WIN_P_IREAD | WIN_P_IWRITE | WIN_P_IEXEC)
#define WIN_P_IRX	(WIN_P_IREAD | WIN_P_IEXEC)

#define WIN_STACKSIZE	0x100000

#define WIN_SIDL	1	/* Process being created by fork. */
#define WIN_SRUN	2	/* Currently runnable. */
#define WIN_SSLEEP	3	/* Sleeping on an address. */
#define WIN_SSTOP	4	/* Process debugging or suspension. */
#define WIN_SZOMB	5	/* Awaiting collection by parent. */
#define WIN_SDEAD	6	/* Process is almost a zombie. */
#define WIN_SONPROC	7	/* Process is currently on a CPU. */

#define STARTF_PS_EXEC	0x00020000

typedef struct _WIN_THREAD_STRUCT {
	/* these are hardcoded (fork.S) */
	ULONG raddr;
	ULONG origin;
	ULONG size;
	ULONG source;
	ULONG offset;
	ULONG dest;
	/* these are dynamic */
	ULONG TaskId;
	HANDLE Token;
	HANDLE Handle;
	DWORD Flags;
	DWORD ThreadId;
	DWORD Result;
} WIN_THREAD_STRUCT;

#define CURRENT	(ULONG)TlsGetValue(__TlsIndex)

/*
 * win_tls.c
 */

#define WIN_GOT_MAX		32

typedef struct _WIN_TLSENTRY {
	LPBYTE Address;
	DWORD Size;
} WIN_TLSENTRY;

static VOID NTAPI TlsMainCRTStartup(LPVOID hinstDLL, DWORD fdwReason, LPVOID lpvReserved);

/* 
 * win_pwd.c
 */

#define WIN_NETENUM_LOCAL		1
#define WIN_NETENUM_GLOBAL		2

typedef struct _WIN_PWENT {
	SID8 UserSid;
	SID8 GroupSid;
	DWORD Privileges;
	DWORD Integrity;
	DWORD Change;
	DWORD Expire;
	WCHAR Account[MAX_NAME];
	WCHAR Domain[MAX_NAME];
	CHAR Password[MAX_NAME];
	CHAR Comment[MAX_COMMENT];
	WCHAR Home[MAX_PATH];
} WIN_PWENT;

typedef struct _WIN_PWENUM {
	DWORD Count;
	DWORD Index;
	DWORD Total;
	DWORD Resume;
	PBYTE Data;
} WIN_PWENUM;

/*
 * win_grp.c
 */

typedef struct _WIN_GRENT {
	SID8 Sid;
	WCHAR Account[MAX_NAME];
	WCHAR Domain[MAX_NAME];
	CHAR Comment[MAX_COMMENT];
	CHAR Members[MAX_TEXT];
} WIN_GRENT;

/*
 * win_uio.c
 */

typedef struct _WIN_IOVEC {
	PVOID Buffer;
	SIZE_T Length;
} WIN_IOVEC;

/*
 * win_unistd.c
 */

#define WIN_PATH_MAX		1024

typedef struct _WIN___TFORK {
} WIN___TFORK;

/*
 * win.c
 */

typedef struct _WIN_GLOBALS {
	UINT SecureLevel;
	CHAR *MsgBuffer;
	UINT MsgBufSize;
	SID8 SidMachine;
	SID8 SidNone;
	LARGE_INTEGER Frequency;	/* result of QueryPerformanceFrequency() */
	LUID AuthId;
	ULONG PageSize;
	DWORD ConMode[2];		/* console mode defaults */
	WCHAR Root[MAX_PATH];
	CHAR Path[WIN_PATH_MAX];
	CHAR SystemRoot[MAX_PATH];
} WIN_GLOBALS;

