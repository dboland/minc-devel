/*
 * Copyright (c) 2026 Daniel Boland <dboland@xs4all.nl>.
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
link_F_NAMEI(WIN_NAMEIDATA *Path, DWORD Flags)
{
	BOOL bResult = FALSE;
	WCHAR szBuffer[MAX_PATH];
	DWORD dwResult;
	LPWSTR pszBase = Path->Resolved;

	if (!(Flags & WIN_FOLLOW)){
		bResult = TRUE;
	}else if (ReadFile(Path->Object, szBuffer, Path->Size, &dwResult, NULL)){
		if (szBuffer[1] == ':'){
			Path->MountId = MOUNTID(szBuffer[0]);	/* nano.exe */
		}else if (szBuffer[1] != '\\'){
			pszBase = Path->Base;
		}
		Path->R = win_wcpcpy(pszBase, szBuffer);
		Path->Base = win_basename(Path->Resolved);
		Path->Last = Path->R - 1;
		Path->FileType = WIN_VREG;
		Path->Attribs = GetFileAttributesW(Path->Resolved);
		bResult = CloseHandle(Path->Object);
	}
	return(bResult);
}
BOOL 
link_F_BIND(LPWSTR FileName, SECURITY_ATTRIBUTES *Attribs, LPWSTR Target)
{
	BOOL bResult = FALSE;
	DWORD dwResult;
	WIN_INODE iNode = {TypeNameVirtual, DEV_TYPE_ROOT, WIN_VLNK, 
		FS_TYPE_DISK, INAMESIZE(Target), 0, 0};
	HANDLE hNode;

	hNode = CreateFileW(FileName, GENERIC_WRITE, FILE_SHARE_READ, 
		Attribs, CREATE_NEW, FILE_CLASS_INODE, NULL);
	if (hNode == INVALID_HANDLE_VALUE){
		return(FALSE);
	}else if (!WriteFile(hNode, &iNode, sizeof(WIN_INODE), &dwResult, NULL)){
		WIN_ERR("WriteFile(%ls): %s\n", FileName, win_strerror(GetLastError()));
	}else if (WriteFile(hNode, Target, iNode.NameSize, &dwResult, NULL)){
		bResult = CloseHandle(hNode);
	}
	return(bResult);
}
