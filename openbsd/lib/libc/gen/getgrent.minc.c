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

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <grp.h>

#include "arch_types.h"

#include <sys/sysctl.h>

#define GRPBUF_MAX	512

char			*_MEMBUF[NGROUPS_MAX];

struct group		_GROUP;
char			_GRPBUF[GRPBUF_MAX];

/************************************************************/

char *
grp_skip(char *p, char c)
{
	while (*p && *p != c){
		p++;
	}
	*p++ = 0;
	return(p);
}
struct group *
grent_posix(struct group *grp, char *buf)
{
	int count = 0;

	grp->gr_name = buf;
	buf = grp_skip(buf, ':');
	grp->gr_passwd = buf;
//	buf = grp_skip(buf, ':');		/* comment (gecos) */
	buf = grp_skip(buf, ':');		/* gid */
	grp->gr_gid = strtoul(buf, &buf, 10);
	if (grp->gr_gid == ROOT_GID){
		grp->gr_gid = 0;
	}
	*buf++ = 0;			/* members */
	while (*buf){
		_MEMBUF[count++] = buf;
		buf = grp_skip(buf, ',');
	}
	_MEMBUF[count] = NULL;
	grp->gr_mem = _MEMBUF;
	return(grp);
}

/************************************************************/

int 
setgroupent(int stayopen)
{
	int mib[3] = {CTL_USER, USER_GRP, GRP_SETGRENT};

	return(sysctl(mib, 3, NULL, NULL, NULL, 0));
}
void 
setgrent(void)
{
	setgroupent(0);
}
void 
endgrent(void)
{
	int mib[3] = {CTL_USER, USER_GRP, GRP_ENDGRENT};

	sysctl(mib, 3, NULL, NULL, NULL, 0);
}
int 
getgrent_r(struct group *grp, char *buf, size_t buflen, struct group **result)
{
	int status = -1;
	int mib[3] = {CTL_USER, USER_GRP, GRP_GETGRENT};

	if (!sysctl(mib, 3, buf, &buflen, NULL, 0)){
		*result = grent_posix(grp, buf);
		status = 0;
	}
	return(status);
}
int 
getgrnam_r(const char *name, struct group *grp, char *buf, size_t buflen, struct group **result)
{
	int status = -1;
	int mib[4] = {CTL_USER, USER_GRP, GRP_GETGRNAM, (int)name};

	if (!name){
		errno = EINVAL;
	}else if (!buf || !result){
		errno = EFAULT;
	}else if (!sysctl(mib, 4, buf, &buflen, NULL, 0)){
		*result = grent_posix(grp, buf);
		status = 0;
	}
	return(status);
}
int 
getgrgid_r(gid_t gid, struct group *grp, char *buf, size_t buflen, struct group **result)
{
	int status = -1;
	int mib[4] = {CTL_USER, USER_GRP, GRP_GETGRGID, gid};

	if (!buf || !result){
		errno = EFAULT;
	}else if (!sysctl(mib, 4, buf, &buflen, NULL, 0)){
		*result = grent_posix(grp, buf);
		status = 0;
	}
	return(status);
}
struct group *
getgrnam(const char *name)
{
	struct group *result;

	if (!getgrnam_r(name, &_GROUP, _GRPBUF, GRPBUF_MAX, &result)){
		return(result);
	}
	return(NULL);
}
struct group *
getgrgid(gid_t gid)
{
	struct group *result;

	if (!getgrgid_r(gid, &_GROUP, _GRPBUF, GRPBUF_MAX, &result)){
		return(result);
	}
	return(NULL);
}
struct group *
getgrent(void)
{
	struct group *result;

	if (!getgrent_r(&_GROUP, _GRPBUF, GRPBUF_MAX, &result)){
		return(result);
	}
	return(NULL);
}
