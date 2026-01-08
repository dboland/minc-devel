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

typedef enum _STRUCT_TYPE{
	STRUCT_UNKNOWN,
	STRUCT_VNODE,
	STRUCT_NAMEI,
	STRUCT_TTY,
	STRUCT_DEVICE,
	STRUCT_MOUNT,
	STRUCT_CFDATA,
	STRUCT_STATFS,
	STRUCT_INODE,
	STRUCT_ACCESS,
	STRUCT_NETFLAGS,
	STRUCT_TASK,
	STRUCT_THREAD,
	STRUCT_TOKEN_PROCESS,
	STRUCT_TOKEN_THREAD,
	STRUCT_ACL_DESKTOP,
	STRUCT_ACL_STATION,
	STRUCT_ACL_OBJECT,
	STRUCT_ACL_FILE,
	STRUCT_ACL_PROCESS,
	STRUCT_SID_RIGHTS,
	STRUCT_SYSTEM_INFO
} STRUCT_TYPE;

extern const CHAR *__STYPE[];
extern const CHAR *__FTYPE[];
extern const CHAR *__FSTYPE[];

LPSTR win_ACL(LPSTR Buffer, LPCSTR Label, PACL Acl, DWORD Type);
LPSTR win_SID(LPSTR Buffer, LPCSTR Label, PSID Sid);
DWORD win_SECURITY_DESCRIPTOR(PSECURITY_DESCRIPTOR Security, DWORD Type, LPSTR Buffer);
DWORD win_TOKEN(HANDLE Token, LPSTR Buffer);
LPSTR win_TOKEN_GROUPS(LPSTR Buffer, LPCSTR Label, PTOKEN_GROUPS Token);
LPSTR win_TOKEN_PRIVILEGES(LPSTR Buffer, LPSTR Label, PTOKEN_PRIVILEGES Privileges);
LPSTR win_SID_RIGHTS(LPSTR Buffer, LPCSTR Label, PSID Sid);
LPSTR win_SYSTEM_INFO(LPSTR Buffer, LPCSTR Label, SYSTEM_INFO *Info);

DWORD vfs_VNODE(WIN_VNODE *Node, LPSTR Buffer);
DWORD vfs_NAMEI(WIN_NAMEIDATA *Path, LPSTR Buffer);
DWORD vfs_TTY(WIN_TTY *Terminal, LPSTR Buffer);
DWORD vfs_DEVICE(WIN_DEVICE *Device, LPSTR Buffer);
DWORD vfs_MOUNT(WIN_MOUNT *Mount, LPSTR Buffer);
DWORD vfs_CFDATA(WIN_CFDATA *Config, LPSTR Buffer);
DWORD vfs_STATFS(WIN_STATFS *Info, LPSTR Buffer);
DWORD vfs_INODE(WIN_INODE *Node, LPSTR Buffer);
DWORD vfs_ACCESS(ACCESS_MASK Access, LPSTR Buffer);
DWORD vfs_NETFLAGS(LONG Events, LPSTR Buffer);
DWORD vfs_TASK(WIN_TASK *Task, LPSTR Buffer);
DWORD vfs_THREAD(WIN_THREAD_STRUCT *Thread, LPSTR Buffer);
DWORD vfs_CAP_CONTROL(WIN_CAP_CONTROL *Control, LPSTR Buffer);

VOID win_ktrace(STRUCT_TYPE Type, LONG Size, PVOID Data);
VOID vfs_ktrace(LPCWSTR Label, STRUCT_TYPE Type, PVOID Data);
