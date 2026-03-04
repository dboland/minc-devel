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

#include <pwd.h>

WIN_PWENUM		__PWDENUM;

/****************************************************/

char *
passwd_posix(char *buf, size_t buflen, WIN_PWENT *WinPwd)
{
	char *result = buf;
	char *shell = "/bin/ksh";

	buf += win_wcstombs(buf, WinPwd->Account, MAX_NAME);
	buf = win_stpcpy(buf, ":");
	buf = win_stpcpy(buf, WinPwd->Password);
	buf = win_stpcpy(buf, ":");
	buf += sprintf(buf, "%lu", rid_posix(&WinPwd->UserSid));
	buf = win_stpcpy(buf, ":");
	buf += sprintf(buf, "%lu", rid_posix(&WinPwd->GroupSid));
	/* login class */
	if (WinPwd->Integrity == SECURITY_MANDATORY_LOW_RID){
		buf = win_stpcpy(buf, ":daemon");
	}else{
		buf = win_stpcpy(buf, ":default");
	}
	buf = win_stpcpy(buf, ":");
	buf += sprintf(buf, "%lu", WinPwd->Change);
	buf = win_stpcpy(buf, ":");
	buf += sprintf(buf, "%lu", WinPwd->Expire);
	buf = win_stpcpy(buf, ":");
	buf = win_stpcpy(buf, WinPwd->Comment);
	buf = win_stpcpy(buf, ":");
	if (*WinPwd->Home){
		buf = pathp_posix(buf, WinPwd->Home);
	}else if (WinPwd->Integrity == SECURITY_MANDATORY_SYSTEM_RID){
		buf = win_stpcpy(buf, "/root");
	}else if (WinPwd->Integrity == SECURITY_MANDATORY_LOW_RID){
		buf = win_stpcpy(buf, "/var/empty");
		shell = "/bin/sh";
	}else{
		buf += 6 + win_wcstombs(win_stpcpy(buf, "/home/"), WinPwd->Account, MAX_NAME);
	}
	buf = win_stpcpy(buf, ":");
	buf = win_stpcpy(buf, shell);
	return(result);
}

/****************************************************/

int 
pwd_PWD_SETPWENT(void)
{
	int result = 0;

	if (!win_setpwent(&__PWDENUM, WIN_NETENUM_LOCAL)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
pwd_PWD_ENDPWENT(void)
{
	if (__PWDENUM.Data){
		win_endpwent(&__PWDENUM);
	}
	return(0);
}
int 
pwd_PWD_GETPWENT(char *buf, size_t buflen)
{
	int result = 0;
	WIN_PWENT pwResult;

	if (!__PWDENUM.Data){
		win_setpwent(&__PWDENUM, WIN_NETENUM_LOCAL);
	}
	if (!win_getpwent(&__PWDENUM, &pwResult)){
		result -= errno_posix(GetLastError());
	}else{
		passwd_posix(buf, buflen, &pwResult);
	}
	return(result);
}
int 
pwd_PWD_GETPWUID(uid_t uid, char *buf, size_t buflen)
{
	int result = 0;
	WIN_PWENT pwResult;
	SID8 sid;

	if (!uid){
		uid = ROOT_UID;
	}
	if (uid < 0){
		result = -EINVAL;
	}else if (!win_getpwuid(rid_win(&sid, uid), &pwResult)){
		result -= errno_posix(GetLastError());
	}else{
		passwd_posix(buf, buflen, &pwResult);
	}
	return(result);
}
int 
pwd_PWD_GETPWNAM(const char *name, char *buf, size_t buflen)
{
	int result = 0;
	WCHAR szAccount[MAX_NAME];
	WIN_PWENT pwResult;

	if (!strncmp(name, "_", 1)){
		result = pwd_PWD_GETPWUID(DAEMON_UID, buf, buflen);
	}else if (!strcmp(name, "root")){
		result = pwd_PWD_GETPWUID(ROOT_UID, buf, buflen);
	}else if (!win_mbstowcs(szAccount, name, MAX_NAME)){
		result = -EINVAL;
	}else if (!win_getpwnam(szAccount, &pwResult)){
		result -= errno_posix(GetLastError());
	}else{
		passwd_posix(buf, buflen, &pwResult);
	}
	return(result);
}
int 
pwd_USER_PWD(const int *name, void *oldp, size_t *oldlenp)
{
	int result = 0;

	switch (name[2]){
		case PWD_SETPWENT:
			result = pwd_PWD_SETPWENT();
			break;
		case PWD_ENDPWENT:
			result = pwd_PWD_ENDPWENT();
			break;
		case PWD_GETPWENT:
			result = pwd_PWD_GETPWENT(oldp, *oldlenp);
			break;
		case PWD_GETPWNAM:
			result = pwd_PWD_GETPWNAM((const char *)name[3], oldp, *oldlenp);
			break;
		case PWD_GETPWUID:
			result = pwd_PWD_GETPWUID(name[3], oldp, *oldlenp);
			break;
		default:
			result = -ENOENT;
	}
	return(result);
}
