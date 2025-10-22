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
dir_close(WIN_VNODE *Node)
{
	BOOL bResult = FALSE;

	if (disk_closedir(Node)){
		bResult = file_close(Node);
	}
	return(bResult);
}
BOOL 
dir_rename(WIN_NAMEIDATA *Path, WIN_NAMEIDATA *Result)
{
	BOOL bResult = FALSE;
	DWORD dwFlags = MOVEFILE_COPY_ALLOWED;

	/* The first MoveFile() is to determine if we are renaming
	 * the same directory. This will yield ERROR_SUCCESS. Otherwise,
	 * we can safely remove the existing one first.
	 */
	if (Path->MountId != Result->MountId){
		SetLastError(ERROR_NOT_SAME_DEVICE);
	}else if (Result->Attribs == -1){
		bResult = MoveFileExW(Path->Resolved, Result->Resolved, dwFlags);
	}else if (Result->FileType != WIN_VDIR){
		SetLastError(ERROR_DIRECTORY);
	}else if (MoveFileExW(Path->Resolved, Result->Resolved, dwFlags)){
		bResult = TRUE;
	}else if (RemoveDirectoryW(Result->Resolved)){
		bResult = MoveFileExW(Path->Resolved, Result->Resolved, dwFlags);
	}
	return(bResult);
}
