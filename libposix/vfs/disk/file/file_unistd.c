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
file_close(WIN_VNODE *Node)
{
	BOOL bResult = FALSE;

	if (!CloseHandle(Node->Handle)){
		WIN_ERR("file_close(%d): %s\n", Node->Handle, win_strerror(GetLastError()));
	}else{
		ZeroMemory(Node, sizeof(WIN_VNODE));
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
file_rename(WIN_NAMEIDATA *Path, WIN_NAMEIDATA *Result)
{
	BOOL bResult = FALSE;
	DWORD dwFlags = MOVEFILE_REPLACE_EXISTING;

	/* We can't use MoveFile() first. When target file is a hard link,
	 * user would get a "corrupted file" message in their GUI (perl.exe).
	 */
	if (*Path->Last == '\\' || *Result->Last == '\\'){	/* GNU conftest.exe */
		SetLastError(ERROR_BAD_PATHNAME);
	}else if (Result->Attribs == -1){
		bResult = MoveFileW(Path->Resolved, Result->Resolved);
	}else if (Result->FileType != WIN_VREG){
		SetLastError(ERROR_FILE_EXISTS);
	}else if (ReplaceFileW(Result->Resolved, Path->Resolved, NULL, 0, NULL, NULL)){
		bResult = TRUE;
	}else if (ERROR_UNABLE_TO_MOVE_REPLACEMENT == GetLastError()){	/* cross-device */
		bResult = MoveFileExW(Path->Resolved, Result->Resolved, dwFlags);
	}else if (ERROR_SHARING_VIOLATION == GetLastError()){	/* same file, different case */
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
file_unlink(WIN_NAMEIDATA *Path)
{
	BOOL bResult = FALSE;

	if (*Path->Last == '\\'){		/* GNU conftest.exe */
		SetLastError(ERROR_BAD_PATHNAME);
	}else{
		bResult = DeleteFileW(Path->Resolved);
	}
	return(bResult);
}
