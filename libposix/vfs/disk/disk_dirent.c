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

static const UCHAR __DTYPE_POSIX[] = {
	WIN_DT_UNKNOWN,
	WIN_DT_REG,
	WIN_DT_DIR,
	WIN_DT_BLK,
	WIN_DT_CHR,
	WIN_DT_LNK,
	WIN_DT_SOCK,
	WIN_DT_FIFO,
	0
};

#define DIRENT_RECSIZE(namelen) \
    ((__builtin_offsetof(WIN_DIRENT, FileName) + (namelen) + 1 + 7) &~ 7)

/************************************************************/

BOOL 
DiskGetEntity(WIN32_FIND_DATAW *Data, WIN_NAMEIDATA *Path, PVOID Buffer, DWORD *Result)
{
	BOOL bResult = TRUE;
	DWORD dwAttribs = Data->dwFileAttributes;
	WIN_DIRENT *pdInfo = Buffer;
	LPWSTR pszType = win_typename(Data->cFileName);
	DWORD dwRecSize, dwNameSize;

	if (FILE_CLASS_INODE == FILE_CLASS(dwAttribs)){
		win_wcscpy(Path->Base, Data->cFileName);
		bResult = disk_lookup(Path, 0);
	}else if (dwAttribs & FILE_ATTRIBUTE_DIRECTORY){
		Path->FileType = WIN_VDIR;
	}else if (!wcscmp(pszType, L".lnk")){
		Path->FileType = WIN_VLNK;
		Path->FSType = FS_TYPE_SHELL;
		*pszType = 0;			/* chop off extension */
	}else{
		Path->FileType = WIN_VREG;
	}
	dwNameSize = win_wcstombs(pdInfo->FileName, Data->cFileName, WIN_MAXNAMLEN);
	dwRecSize = DIRENT_RECSIZE(dwNameSize);
	pdInfo->FileId = (DWORDLONG)Path->Index;
	pdInfo->Offset = (DWORDLONG)dwRecSize;
	pdInfo->RecSize = dwRecSize;
	pdInfo->NameSize = dwNameSize;
	pdInfo->FileType = __DTYPE_POSIX[Path->FileType];
	*Result = dwRecSize;
	return(bResult);
}

/****************************************************/

BOOL 
disk_closedir(WIN_VNODE *Node)
{
	BOOL bResult = FALSE;

	if (!Node->Object){
		return(TRUE);
	}else if (!FindClose(Node->Object)){
		WIN_ERR("FindClose(%d): %s\n", Node->Object, win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
disk_rewinddir(WIN_NAMEIDATA *Path, WIN32_FIND_DATAW *Data)
{
	BOOL bResult = FALSE;
	HANDLE hResult = NULL;

	hResult = FindFirstFileW(Path->Resolved, Data);
	if (hResult == INVALID_HANDLE_VALUE){
		WIN_ERR("FindFirstFile(%ls): %s\n", Path->Resolved, win_strerror(GetLastError()));
	}else{
		Path->Object = hResult;
		Path->Index = 1;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
disk_readdir(WIN_NAMEIDATA *Path, WIN32_FIND_DATAW *Data)
{
	BOOL bResult = FALSE;

	/* Windows does some type of garbage collection on the search handle.
	 * When not closing it immediately after the last FindNextFile(),
	 * FindClose() yields ERROR_NOACCESS (gmake.exe).
	 */
	if (!Path->Object){
		bResult = disk_rewinddir(Path, Data);
	}else if (FindNextFileW(Path->Object, Data)){
		bResult = TRUE;
	}else if (ERROR_NO_MORE_FILES != GetLastError()){
		WIN_ERR("FindNextFile(%d): %s\n", Path->Object, win_strerror(GetLastError()));
	}else if (!FindClose(Path->Object)){
		WIN_ERR("FindClose(%d): %s\n", Path->Object, win_strerror(GetLastError()));
	}else{
		Path->Object = NULL;
		Path->Index = -1;
	}
	return(bResult);
}
BOOL 
disk_getdents(WIN_NAMEIDATA *Path, PVOID Buffer, DWORD Size, DWORD *Result)
{
	BOOL bResult = TRUE;
	DWORD dwResult = 0;
	LONG lSize = Size;
	WIN32_FIND_DATAW wfData;
	DWORD dwSize;

	if (Path->Index == -1){
		SetLastError(ERROR_NO_MORE_FILES);
	}else while (lSize >= sizeof(WIN_DIRENT)){
		if (!disk_readdir(Path, &wfData)){
			break;
		}else if (wfData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN){
			continue;
		}else if (!DiskGetEntity(&wfData, Path, Buffer, &dwSize)){
			bResult = FALSE;
			break;
		}else{
			dwResult += dwSize;
			lSize -= dwSize;
			Buffer += dwSize;
			Path->Index++;
		}
	}
	*Result = dwResult;
	return(bResult);
}
