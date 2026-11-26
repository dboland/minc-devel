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

#include <winbase.h>

/****************************************************/

BOOL 
pdo_open(WIN_NAMEIDATA *Path, WIN_FLAGS *Flags, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;

	if (!PdoOpenFile(Path, Flags, Result)){
		SetLastError(ERROR_DEVICE_NOT_AVAILABLE);
	}else switch (Result->DeviceType){
		case DEV_TYPE_CONSOLE:
			bResult = ptm_open(DEVICE(DEV_TYPE_CONSOLE), Flags, Result);
			break;
		case DEV_TYPE_TTY:
			bResult = tty_open(__CTTY, Flags, Result);
			break;
		case DEV_TYPE_NULL:
			bResult = null_open(Flags, Result);
			break;
		case DEV_TYPE_PTM:
			bResult = ptm_open(pdo_attach(DEV_TYPE_PTY), Flags, Result);
			break;
		case DEV_TYPE_STDIN:		/* sort.exe */
			*Result = __Nodes[__Process->TaskId][0];
			bResult = TRUE;
			break;
		case DEV_TYPE_STDOUT:
			*Result = __Nodes[__Process->TaskId][1];
			bResult = TRUE;
			break;
		case DEV_TYPE_STDERR:
			*Result = __Nodes[__Process->TaskId][2];
			bResult = TRUE;
			break;
		default:
			bResult = TRUE;
	}
	return(bResult);
}
BOOL 
pdo_close(WIN_VNODE *Node)
{
	BOOL bResult = FALSE;

	switch (Node->FileType){
		case WIN_VSOCK:
			bResult = sock_close(Node);
			break;
		default:
			bResult = object_close(Node);
	}
	return(bResult);
}
BOOL 
pdo_read(WIN_TASK *Task, WIN_DEVICE *Device, LPSTR Buffer, LONG Size, DWORD *Result)
{
	BOOL bResult = FALSE;

	switch (Device->DeviceType){
		case DEV_TYPE_ROUTE:
			bResult = route_read(Device, Buffer, Size, Result);
			break;
		case DEV_TYPE_RANDOM:			/* pwgen.exe */
		case DEV_TYPE_URANDOM:
			bResult = rand_read(Buffer, Size, Result);
			break;
		default:
			SetLastError(ERROR_BAD_DEVICE);
	}
	return(bResult);
}
BOOL 
pdo_write(WIN_DEVICE *Device, LPCSTR Buffer, DWORD Size, DWORD *Result)
{
	BOOL bResult = FALSE;

	switch (Device->DeviceType){
		case DEV_TYPE_ROUTE:
			bResult = route_write(Device, Buffer, Size, Result);
			break;
		default:
			SetLastError(ERROR_BAD_DEVICE);
	}
	return(bResult);
}
BOOL 
pdo_unlink(WIN_NAMEIDATA *Path)
{
	BOOL bResult = FALSE;

	if (*Path->Last == '\\'){		/* GNU conftest.exe */
		SetLastError(ERROR_BAD_PATHNAME);
	}else if (CloseHandle(Path->Object)){
		bResult = DeleteFileW(Path->Resolved);
	}
	return(bResult);
}
BOOL 
pdo_revoke(WIN_DEVICE *Device)
{
	BOOL bResult = FALSE;

	switch (Device->DeviceType){
		case DEV_TYPE_CONSOLE:
		case DEV_TYPE_PTY:
			SetLastError(ERROR_CTX_NOT_CONSOLE);
			break;
		default:
			SetLastError(ERROR_BAD_DEVICE);
	}
	return(bResult);
}
BOOL 
pdo_access(WIN_NAMEIDATA *Path, ACCESS_MASK Access)
{
	BOOL bResult = FALSE;
	PSECURITY_DESCRIPTOR psd;

	if (!Access){
		return(Path->Attribs != -1);
	}else if (!win_acl_get_fd(Path->Object, &psd)){
		return(FALSE);
	}else if (CloseHandle(Path->Object)){
		bResult = VfsAccessCheck(psd, Access);
	}
	LocalFree(psd);		/* resets last error */
	return(bResult);
}
