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
pdo_F_LOOKUP(HANDLE Handle, DWORD Flags, WIN_NAMEIDATA *Result)
{
	BOOL bResult = TRUE;

	if (Flags & WIN_NEEDHANDLE){
		Result->Object = Handle;
	}else{
		bResult = CloseHandle(Handle);
	}
	return(bResult);
}
BOOL 
pdo_F_BIND(LPWSTR FileName, DWORD FileType, SECURITY_ATTRIBUTES *Attribs, DWORD DeviceId)
{
	BOOL bResult = FALSE;
	DWORD dwResult;
	WIN_INODE iNode = {TypeNameVirtual, DeviceId, FileType, 
		FS_TYPE_PDO, 0, 0, 0};
	HANDLE hNode;

	hNode = CreateFileW(FileName, GENERIC_WRITE, FILE_SHARE_READ, 
		Attribs, CREATE_NEW, FILE_CLASS_INODE, NULL);
	if (hNode == INVALID_HANDLE_VALUE){
		return(FALSE);
	}else if (WriteFile(hNode, &iNode, sizeof(WIN_INODE), &dwResult, NULL)){
		bResult = CloseHandle(hNode);
	}
	return(bResult);
}

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
			win_memcpy(Result, &__Process->Node[0], sizeof(WIN_VNODE));
			bResult = TRUE;
			break;
		case DEV_TYPE_STDOUT:
			win_memcpy(Result, &__Process->Node[1], sizeof(WIN_VNODE));
			bResult = TRUE;
			break;
		case DEV_TYPE_STDERR:
			win_memcpy(Result, &__Process->Node[2], sizeof(WIN_VNODE));
			bResult = TRUE;
			break;
		case DEV_TYPE_WINDOWS:
			Result->Event = __Interrupt;
		default:
			bResult = TRUE;
	}
	return(bResult);
}
