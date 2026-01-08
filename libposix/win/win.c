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

/* exclude msvcrt.dll headers */
#define WIN32_LEAN_AND_MEAN

/* shlobj.h */
#define _WIN32_WINNT	_WIN32_WINNT_WINXP

/* ddk/winddk.h (prevent dllimport decoration) */
#define NTOSAPI

#include <windows.h>
#include <ddk/ntapi.h>

#include <time.h>

#include "ntdll_posix.h"
#include "msvc_posix.h"
#include "win_types.h"

#define WIN_ERR			msvc_printf

SID8 SidNull			= {1, 1, {0, 0, 0, 0, 0, 0}, 0};
SID8 SidEveryone		= {1, 1, {0, 0, 0, 0, 0, 1}, 0};
SID8 SidLocal			= {1, 1, {0, 0, 0, 0, 0, 2}, 0};
SID8 SidConsoleLogon		= {1, 1, {0, 0, 0, 0, 0, 2}, 1};
SID8 SidCreator			= {1, 1, {0, 0, 0, 0, 0, 3}, 0};
SID8 SidNTAuth			= {1, 1, {0, 0, 0, 0, 0, 5}, 0};
SID8 SidBatch			= {1, 1, {0, 0, 0, 0, 0, 5}, 3};
SID8 SidInteractive		= {1, 1, {0, 0, 0, 0, 0, 5}, 4};
SID8 SidLogonSession		= {1, 3, {0, 0, 0, 0, 0, 5}, 5, 0, 0};
SID8 SidService			= {1, 1, {0, 0, 0, 0, 0, 5}, 6};
SID8 SidAuthenticated		= {1, 1, {0, 0, 0, 0, 0, 5}, 11};
SID8 SidRestricted		= {1, 1, {0, 0, 0, 0, 0, 5}, 12};
SID8 SidThisOrganization	= {1, 1, {0, 0, 0, 0, 0, 5}, 15};
SID8 SidSystem			= {1, 1, {0, 0, 0, 0, 0, 5}, 18};
SID8 SidLocalService		= {1, 1, {0, 0, 0, 0, 0, 5}, 19};
SID8 SidBuiltinAuth		= {1, 2, {0, 0, 0, 0, 0, 5}, 32, 0};
SID8 SidAdmins			= {1, 2, {0, 0, 0, 0, 0, 5}, 32, 544};
SID8 SidUsers			= {1, 2, {0, 0, 0, 0, 0, 5}, 32, 545};
SID8 SidGuests			= {1, 2, {0, 0, 0, 0, 0, 5}, 32, 546};
SID8 SidPowerUsers		= {1, 2, {0, 0, 0, 0, 0, 5}, 32, 547};

/* Vista */

SID8 SidAuthentication		= {1, 2, {0, 0, 0, 0, 0, 5}, 64, 0};
SID8 SidNTLMAuth		= {1, 2, {0, 0, 0, 0, 0, 5}, 64, 10};
SID8 SidNTService		= {1, 2, {0, 0, 0, 0, 0, 5}, 80, 0};
SID8 SidTrustedInstaller	= {1, 6, {0, 0, 0, 0, 0, 5}, 80, 0x38FB89B5, 0xCBC28419, 0x6D236C5C, 0x6E770057, 0x876402C0};
SID8 SidLocalUser		= {1, 1, {0, 0, 0, 0, 0, 5}, 113};
SID8 SidLocalAdmin		= {1, 1, {0, 0, 0, 0, 0, 5}, 114};
SID8 SidOtherOrganization	= {1, 1, {0, 0, 0, 0, 0, 5}, 1000};
SID8 SidOtherDomain		= {1, 2, {0, 0, 0, 0, 0, 6}, 0, 0};
SID8 SidPackage			= {1, 2, {0, 0, 0, 0, 0, 15}, 2, 0};
SID8 SidPackageAll		= {1, 2, {0, 0, 0, 0, 0, 15}, 2, 1};
SID8 SidPackageRestrict		= {1, 2, {0, 0, 0, 0, 0, 15}, 2, 2};
SID8 SidCapability		= {1, 2, {0, 0, 0, 0, 0, 15}, 3, 0};
SID8 SidIntegrityAuth		= {1, 1, {0, 0, 0, 0, 0, 16}, 0};
SID8 SidIntegrityLow		= {1, 1, {0, 0, 0, 0, 0, 16}, 0x1000};
SID8 SidIntegrityMedium		= {1, 1, {0, 0, 0, 0, 0, 16}, 0x2000};
SID8 SidIntegrityHigh		= {1, 1, {0, 0, 0, 0, 0, 16}, 0x3000};
SID8 SidIntegritySystem		= {1, 1, {0, 0, 0, 0, 0, 16}, 0x4000};

WIN_GLOBALS	*__Globals;
SID8		*__SidMachine;
SID8		*__SidNone;
LARGE_INTEGER	*__Frequency;

#include "win_string.c"
#include "win_wchar.c"
#include "win_libgen.c"
#include "win_stdlib.c"
#include "win_capability.c"
#include "win_syscall.c"
#include "win_acl.c"
#include "win_pwd.c"
#include "win_fcntl.c"
#include "win_unistd.c"
#include "win_grp.c"
#include "win_mman.c"
#include "win_uio.c"
#include "win_signal.c"
#include "win_time.c"
#include "win_sysctl.c"
#include "win_dlfcn.c"

/************************************************************/

VOID 
win_init(WIN_GLOBALS *Globals, HINSTANCE Instance)
{
	SYSTEM_INFO sInfo;
	CHAR szRoot[MAX_PATH];
	LPSTR psz;
	DWORD dwSize;

	GetSystemInfo(&sInfo);
	Globals->PageSize = sInfo.dwPageSize;

	if (!GetModuleFileNameW(Instance, Globals->Root, MAX_PATH)){
		WIN_ERR("GetModuleFileName(0x%x): %s\n", Instance, win_strerror(GetLastError()));
	}else{
		win_dirname(win_dirname(Globals->Root));
	}

	AclInit(&Globals->SidMachine, &Globals->SidNone);
	TimeInit(&Globals->Frequency);
	CapInit(&Globals->AuthId);

	win_wcstombs(szRoot, Globals->Root, MAX_PATH);
	psz = win_stpcpy(Globals->Path, "Path=");
	psz = win_stpcpy(win_stpcpy(psz, szRoot), "\\sbin;");
	psz = win_stpcpy(win_stpcpy(psz, szRoot), "\\usr\\lib;");
	psz = win_stpcpy(win_stpcpy(psz, szRoot), "\\usr\\libexec;");
	dwSize = WIN_PATH_MAX - (psz - Globals->Path);
	GetEnvironmentVariable("Path", psz, dwSize);
	psz = win_stpcpy(Globals->SystemRoot, "SystemRoot=");
	GetEnvironmentVariable("SystemRoot", psz, MAX_PATH);
}
