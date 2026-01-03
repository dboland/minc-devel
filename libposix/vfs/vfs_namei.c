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

WCHAR 
PathRead(WIN_NAMEIDATA *Path, DWORD Flags)
{
	WCHAR *S = Path->S;
	WCHAR *R = Path->R;
	WCHAR C;

	*R++ = '\\';
	Path->Base = R;
	while (C = *S){
		S++;
		if (C == '/'){
			break;
		}else if (C == ':'){	/* perl.exe */
			C = '_';
		}
		*R++ = C;
	}
	*R = 0;
	Path->S = S;
	Path->R = R;
	if (Flags & WIN_STRIPSLASHES){
		C = *S;
	}
	return(C);
}
VOID 
PathCopy(WIN_NAMEIDATA *Path)
{
	WCHAR *S = Path->S;
	WCHAR *R = Path->R;
	WCHAR C;

	Path->Base = R;
	while (C = *S){
		S++;
		if (C == '/'){
			C = '\\';
		}else if (C == ':'){
			C = '_';
		}
		*R++ = C;
	}
	*R = 0;
	Path->S = S;
	Path->R = R;
}
BOOL 
PathGlob(WIN_NAMEIDATA *Path, DWORD Flags)
{
	BOOL bResult = TRUE;
	DWORD dwAttribs = GetFileAttributesW(Path->Resolved);

	Path->Attribs = dwAttribs;
	if (dwAttribs == -1){
		bResult = FALSE;
	}else switch (FILE_CLASS(dwAttribs)){
		case FILE_CLASS_INODE:
			bResult = disk_lookup(Path, Flags);
			break;
		case FILE_CLASS_MOUNT:
			bResult = drive_lookup(Path, Flags);
			break;
		case FILE_CLASS_ROOT:
			break;
	}
	return(bResult);
}
VOID 
PathOpen(WIN_NAMEIDATA *Path, LPWSTR Source, DWORD Flags)
{
	Path->Attribs = -1;
	Path->FileType = WIN_VREG;
	Path->S = Source;
	Path->Flags = Flags;
}
VOID 
PathClose(WIN_NAMEIDATA *Path, DWORD Flags)
{
	BOOL bResult = TRUE;

	if (PathGlob(Path, Flags)){
		if (Path->Attribs & FILE_ATTRIBUTE_DIRECTORY){
			Path->FileType = WIN_VDIR;
		}
	}else if (SHGlobType(L".exe", Path)){
		Path->FileType = WIN_VREG;
	}else if (SHGlobLink(Path, Flags)){
		Path->FileType = WIN_VLNK;
//	}else{
//		vfs_ktrace("PathClose", STRUCT_NAMEI, Path);
	}
	Path->Last = Path->R - 1;
}

/****************************************************/

WIN_NAMEIDATA *
vfs_lookup(WIN_NAMEIDATA *Path, LPWSTR Source, DWORD Flags)
{
	PathOpen(Path, Source, Flags);
	if (Flags & WIN_PATHCOPY){	/* vfs_symlink() */
		PathCopy(Path);
	}else while (PathRead(Path, Flags)){
		if (!PathGlob(Path, WIN_FOLLOW)){
			*Path->R++ = '\\';
			PathCopy(Path);
			Path->Last = Path->R - 1;
			return(Path);
		}
	}
	PathClose(Path, Flags);
	return(Path);
}
BOOL 
vfs_namei(HANDLE Handle, DWORD Index, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;
	DWORD dwType = GetFileType(Handle);
	CHAR szBuffer[MAX_NAME];

	switch (dwType){
		case FS_TYPE_DISK:
			bResult = disk_namei(Handle, Result);
			break;
		case FS_TYPE_CHAR:
			bResult = char_namei(Handle, Index, Result);
			break;
		case FS_TYPE_PIPE:
			bResult = pipe_namei(Handle, Result);
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
