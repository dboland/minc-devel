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

#include <winsock2.h>

#include <ddk/ntapi.h>
#include <ddk/ntifs.h>		/* Object types */

#include <winerror.h>
#include <ntsecapi.h>		/* LSA_HANDLE */
#include <lmcons.h>		/* UNLEN */
//#include <time.h>

#include "win/ntdll.h"
#include "win_posix.h"
#include "msvc_posix.h"
#include "vfs_types.h"
#include "char_types.h"

#define WIN_ERR		msvc_printf

extern WIN_PSTRING	*__Strings;

LPSTR win_strsid(PSID Sid);
LPSTR win_strerror(HRESULT Error);
LPSTR nt_strerror(NTSTATUS Status);

LPSTR WinFlagName(LPSTR Buffer, DWORD Flag, LPCSTR Name, DWORD Mask, DWORD *Remain);
LPSTR WinAceFlags(LPSTR Buffer, LPCSTR Label, BYTE Flags);
LPSTR WinFileAccess(LPSTR Buffer, ACCESS_MASK Access, DWORD Type);
LPSTR WinSecurityFlags(LPSTR Buffer, LPCSTR Label, WORD Flags);
LPSTR WinGroupAttribs(LPSTR Buffer, LPCSTR Label, DWORD Attribs);
LPSTR WinPrivilegeAttribs(LPSTR Buffer, LPCSTR Label, DWORD Attributes);

LPSTR VfsFileAttribs(LPSTR Buffer, DWORD Attribs);
LPSTR VfsFileFlags(LPSTR Buffer, DWORD Flags);
LPSTR VfsPathFlags(LPSTR Buffer, LPCSTR Label, DWORD Flags);
LPSTR VfsVolumeFlagsHigh(LPSTR Buffer, LPCSTR Label, DWORD Flags);
LPSTR VfsVolumeFlagsLow(LPSTR Buffer, LPCSTR Label, DWORD Flags);
LPSTR VfsTaskFlags(LPSTR Buffer, LPCSTR Label, DWORD Flags);
LPSTR VfsInputMode(LPSTR Buffer, LPCSTR Label, DWORD Mode);
LPSTR VfsScreenMode(LPSTR Buffer, LPCSTR Label, DWORD Mode);
LPSTR VfsNetEvents(LPSTR Buffer, LONG NetworkEvents);

#include "libtrace.h"
