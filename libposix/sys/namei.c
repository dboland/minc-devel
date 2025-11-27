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

#include <sys/namei.h>

#define PROCESS_ROOT	L"\\\\.\\GLOBALROOT\\"

/****************************************************/

char *
pathnp_posix(char *dest, LPCWSTR Source, LONG Size, BOOL EndPtr)
{
	char *result = dest;
	char *type = NULL;
	char c;
	char buf[PATH_MAX], *src = buf;
	DWORD dwType = 0;

	if (!wcsncmp(Source, PROCESS_ROOT, 14)){
		Source += 14;
	}
	win_wcstombs(src, Source, Size);
	if (src[1] == ':'){		/* MinGW ld.exe */
		dest = win_stpcpy(dest, "/mnt/");
		Size -= 5;
	}else if (src[0] == '\\'){
		dest = win_stpcpy(dest, "/proc");
		Size -= 5;
	}else if (src[1] == '\\'){
		src++;
	}
	while (c = *src++){
		Size--;
		if (Size < 1){
			break;
		}else if (c == ':'){
			continue;
		}else if (c == '\\'){
			c = '/';
		}else if (c == '.'){
			type = dest;
		}
		*dest++ = c;
	}
	*dest = 0;
	dwType = type ? *(DWORD *)type : 0;
	if (dwType == TypeNameLink){
		*type = 0;		/* chop ".lnk" extension */
	}
	if (EndPtr){
		result = dest;
	}
	return(result);
}
char *
path_posix(char *dest, LPCWSTR Source)
{
	return(pathnp_posix(dest, Source, PATH_MAX, FALSE));
}
char *
pathn_posix(char *dest, LPCWSTR Source, DWORD Size)
{
	return(pathnp_posix(dest, Source, Size, FALSE));
}
char *
pathp_posix(char *dest, LPCWSTR Source)
{
	return(pathnp_posix(dest, Source, PATH_MAX, TRUE));
}

/****************************************************/

const char *
root_win(WIN_NAMEIDATA *Result, const char *path)
{
	if (!strncmp(path, "/proc/", 6)){		/* vim.exe */
		Result->R = win_wcpcpy(Result->Resolved, PROCESS_ROOT);
		path += 6;
	}else{
		Result->R = win_wcpcpy(Result->Resolved, L".");
		path++;
	}
	return(path);
}
WIN_NAMEIDATA *
pathat_win(WIN_NAMEIDATA *Result, int dirfd, const char *path, int atflags)
{
	WCHAR szSource[WIN_PATH_MAX];
	size_t size;
	WIN_TASK *pwTask = &__Tasks[CURRENT];
	DWORD dwFlags = WIN_FOLLOW;

	if (pwTask->TracePoints & KTRFAC_NAMEI){
		ktrace_NAMEI(pwTask, path, strlen(path));
	}
	if (atflags & AT_SYMLINK_NOFOLLOW){
		dwFlags = WIN_NOFOLLOW;
	}
	if (atflags & AT_NOCROSS){
		dwFlags |= WIN_NOCROSSMOUNT;
	}
	if (atflags & AT_REMOVEDIR){
		dwFlags |= WIN_REQUIREDIR;
	}
	if (atflags & AT_NOSLASH){
		dwFlags |= WIN_STRIPSLASHES;
	}
	if (atflags & AT_SYMLINK){
		dwFlags |= WIN_ISSYMLINK;
	}
	if (atflags & AT_INODE){
		dwFlags |= WIN_NEEDHANDLE;
	}
	if (atflags & AT_LOCKLEAF){
		dwFlags |= WIN_LOCKLEAF;
	}

	Result->MountId = 0;
	Result->DeviceId = DEV_TYPE_ROOT;
	Result->FSType = FS_TYPE_DISK;
	Result->R = Result->Resolved;
	Result->Base = Result->R;
	Result->Owner = &pwTask->UserSid;
	Result->Group = &pwTask->GroupSid;

	if (dirfd > 0 && dirfd < OPEN_MAX){
		vfs_F_GETPATH(&pwTask->Node[dirfd], &pwTask->UserSid, &pwTask->GroupSid, Result);

	}else if (path[0] == '/'){
		path = root_win(Result, path);

	}else if (path[1] == ':'){		/* MSYS sh.exe */
		Result->MountId = MOUNTID(path[0]);
		*Result->R++ = *path++;
		*Result->R++ = *path++;
		if (path[0]) path++;

	}else if (dirfd == AT_FDCWD){
		Result->MountId = pwTask->MountId;
		Result->R = win_wcpcpy(Result->R, __Strings[pwTask->TaskId].Path);

	}else{
		dwFlags |= WIN_PATHCOPY;

	}

	size = WIN_PATH_MAX - (Result->R - Result->Resolved);
	win_mbstowcs(szSource, path, size);

	return(vfs_lookup(Result, szSource, dwFlags));

}
WIN_NAMEIDATA *
path_win(WIN_NAMEIDATA *Result, const char *path, int flags)
{
	int atflags = AT_SYMLINK_FOLLOW;

	if (flags & O_NOFOLLOW){
		atflags |= AT_SYMLINK_NOFOLLOW;
	}
	if (flags & O_NOCROSS){
		atflags |= AT_NOCROSS;
	}
	if (flags & O_DIRECTORY){
		atflags |= AT_REMOVEDIR;
	}
	if (flags & O_NOSLASH){
		atflags |= AT_NOSLASH;
	}
	if (flags & O_SYMLINK){
		atflags |= AT_SYMLINK;
	}
	if (flags & O_INODE){
		atflags |= AT_INODE;
	}
	return(pathat_win(Result, AT_FDCWD, path, atflags));
}
