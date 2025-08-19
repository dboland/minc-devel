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

#include <grp.h>

WIN_PWENUM		__GRPENUM;

/****************************************************/

char *
group_posix(char *buf, size_t buflen, WIN_GRENT *Group)
{
	char *result = buf;

	win_bzero(buf, buflen);		/* userinfo.exe */
	buf += win_wcstombs(buf, Group->Account, MAX_NAME);
	buf = win_stpcpy(buf, ":*:");
//	buf = win_stpcpy(buf, Group->Comment);
//	buf = win_stpcpy(buf, ":");
	buf += sprintf(buf, "%lu", rid_posix(&Group->Sid));
	buf = win_stpcpy(buf, ":");
	buf = win_stpcpy(buf, Group->Members);
	return(result);
}

/****************************************************/

int 
grp_GRP_SETGRENT(void)
{
	int result = 0;

	if (!win_setgrent(&__GRPENUM, WIN_NETENUM_LOCAL)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
grp_GRP_ENDGRENT(void)
{
	if (__GRPENUM.Data){
		win_endgrent(&__GRPENUM);
	}
	return(0);
}
int 
grp_GRP_GETGRENT(char *buf, size_t buflen)
{
	int result = 0;
	WIN_GRENT wgResult;

	if (!__GRPENUM.Data){
		win_setgrent(&__GRPENUM, WIN_NETENUM_LOCAL);
	}
	if (!win_getgrent(&__GRPENUM, &wgResult)){
		result -= errno_posix(GetLastError());
	}else{
		group_posix(buf, buflen, &wgResult);
	}
	return(result);
}
int 
grp_GRP_GETGRGID(gid_t gid, char *buf, size_t buflen)
{
	int result = 0;
	WIN_GRENT wgResult;
	SID8 sid;

	if (!gid){
		gid = ROOT_GID;
	}
	if (gid < 0){
		result = -EINVAL;
	}else if (!win_getgrgid(rid_win(&sid, gid), &wgResult)){
		result -= errno_posix(GetLastError());
	}else{
		group_posix(buf, buflen, &wgResult);
	}
	return(result);
}
int 
grp_GRP_GETGRNAM(const char *name, char *buf, size_t buflen)
{
	int result = 0;
	WCHAR szAccount[MAX_NAME];
	WIN_GRENT wgResult;

	if (!strncmp(name, "_", 1)){
		result = grp_GRP_GETGRGID(DAEMON_GID, buf, buflen);
	}else if (!strcmp(name, "bin")){
		result = grp_GRP_GETGRGID(BIN_GID, buf, buflen);
	}else if (!strcmp(name, "wheel")){
		result = grp_GRP_GETGRGID(ROOT_GID, buf, buflen);
	}else if (!strcmp(name, "operator")){
		result = grp_GRP_GETGRGID(OPERATOR_GID, buf, buflen);
	}else if (!win_mbstowcs(szAccount, name, MAX_NAME)){
		result = -EINVAL;
	}else if (!win_getgrnam(szAccount, &wgResult)){
		result -= errno_posix(GetLastError());
	}else{
		group_posix(buf, buflen, &wgResult);
	}
	return(result);
}
int 
grp_GRP_GETGROUPLIST(const char *user, gid_t group, gid_t *groups, int *ngroups)
{
	int result = 0;
	WIN_PWENT pwEntry;
	WCHAR szAccount[MAX_NAME];
	SID8 *psGroups = NULL;
	SID8 sid;
	DWORD dwCount = 0;
	int index = 0;
	gid_t next;

	if (!group){
		group = ROOT_GID;
	}
	if (!win_mbstowcs(szAccount, user, MAX_NAME)){
		result = -EINVAL;
	}else if (!win_getpwnam(szAccount, &pwEntry)){
		result -= errno_posix(GetLastError());
	}else if (!win_getgrouplist(&pwEntry, rid_win(&sid, group), &psGroups, &dwCount)){
		result -= errno_posix(GetLastError());
	}else if (*ngroups < dwCount){
		result = -EINVAL;
	}else while (index < dwCount){
		next = rid_posix(psGroups);
		if (next == ROOT_GID){
			groups[index] = 0;
		}else{
			groups[index] = next;
		}
		psGroups++;
		index++;
	}
	win_free(psGroups);
	*ngroups = dwCount;
	return(result);
}

/****************************************************/

int 
grp_USER_GRP(const int *name, void *oldp, size_t *oldlenp)
{
	int result = 0;

	switch (name[2]){
		case GRP_SETGRENT:
			result = grp_GRP_SETGRENT();
			break;
		case GRP_ENDGRENT:
			result = grp_GRP_ENDGRENT();
			break;
		case GRP_GETGRENT:
			result = grp_GRP_GETGRENT(oldp, *oldlenp);
			break;
		case GRP_GETGRNAM:
			result = grp_GRP_GETGRNAM((const char *)name[3], oldp, *oldlenp);
			break;
		case GRP_GETGRGID:
			result = grp_GRP_GETGRGID(name[3], oldp, *oldlenp);
			break;
		case GRP_GETGROUPLIST:
			result = grp_GRP_GETGROUPLIST((const char *)name[3], name[4], oldp, oldlenp);
			break;
		default:
			result = -ENOENT;
	}
	return(result);
}
