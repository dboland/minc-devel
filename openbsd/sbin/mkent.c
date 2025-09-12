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
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <wchar.h>

#include <sys/param.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/sysctl.h>

#include "win/windows.h"
#include "win/winsock2.h"
#include "win/iphlpapi.h"
#include "win/aclapi.h"
#include "win_posix.h"
#include "vfs_posix.h"
#include "ws2_posix.h"
#include "msvc_posix.h"
#include "bsd_posix.h"
#include "arch_posix.h"

extern __import SID8 SidSystem;
extern __import SID8 SidService;
extern __import SID8 SidAdmins;
extern __import SID8 SidPackageRestrict;

#define WIN_ERR			printf
#define MAX_PWDBUF		WIN_PATH_MAX

char 	_PWDBUF[MAX_PWDBUF];

int _verbose;
int _paths;

/****************************************************/

void 
print_fsent(WIN_CFDATA *Config, struct statfs *info)
{
	char line[255], *l = line;
	mode_t mode;

	l += sprintf(l, "%s", info->f_mntfromname);
	l += sprintf(l, "\t%s", info->f_mntonname);
	l += sprintf(l, "\t%s", info->f_fstypename);

	if (info->f_flags & MNT_RDONLY){
		l += sprintf(l, "\tro");
	}else{
		l += sprintf(l, "\trw");
	}
	if (info->f_flags & MNT_NOSUID){
		l += sprintf(l, ",nosuid");
	}
	if (info->f_flags & MNT_DOOMED){
		l += sprintf(l, ",noauto");
	}

	printf("%s\t0\t0\n", line);	/* frequency, passno (1=root, 2=other) */

	if (_paths){
		mkdir(info->f_mntonname, 00700);
	}
}
void 
print_usage(char *prog)
{
	printf("Usage: %s [options] COMMAND\n", prog);
	printf("\nCommands\n");
	printf(" passwd\t\t\tprint entries for /etc/master.passwd\n");
	printf(" group\t\t\tprint entries for /etc/group\n");
	printf(" fstab\t\t\tprint entries for /etc/fstab\n");
	printf(" resolv\t\t\tprint entries for /etc/resolv.conf\n");
	printf(" pdo\t\t\tWindows PDO device table (physical drivers)\n");
	printf(" fdo\t\t\tWindows FDO device table (function drivers)\n");
	printf(" vol\t\t\tWindows volume table\n");
	printf(" link\t\t\tWindows link table\n");
	printf(" drive\t\t\tWindows drive table\n");
	printf(" npf\t\t\tNetGroup Packet Filter table\n");
	printf(" ndis\t\t\tWindows network driver table\n");
	printf(" if\t\t\tWindows network adapter table\n");
	printf("\nOptions\n");
	printf(" -p, --path\t\tcreate paths while printing\n");
	printf(" -v, --verbose\t\tbe verbose if applicable\n");
}

/****************************************************/

void 
mk_group(FILE *stream)
{
	int mib[3] = {CTL_USER, USER_GRP, GRP_GETGRENT};
	WIN_GRENT wGroup;
	char buf[MAX_TEXT];
	size_t size = MAX_TEXT;
	WIN_PWENUM wnInfo = {0};
	DWORD wksType = 0;
	DWORD dwSize;
	SID8 sidBuf;

	while (wksType++ < WinWellKnownSidTypeCount){
		dwSize = sizeof(SID8);
		if (CreateWellKnownSid(wksType, NULL, &sidBuf, &dwSize)){
			if (sidBuf.SubAuthority[0] == SECURITY_BUILTIN_DOMAIN){	/* 32 (Builtin) */
				continue;
			}else if (!win_getgrgid(&sidBuf, &wGroup)){
				continue;
			}
		}else if (wksType != WinApplicationPackageAuthoritySid){	/* NT */
			continue;
		}else if (!win_getgrgid(&SidPackageRestrict, &wGroup)){		/* Vista */
			continue;
		}
		fprintf(stream, "%s\n", group_posix(buf, MAX_TEXT, &wGroup));
	}
	while (!sysctl(mib, 3, buf, &size, NULL, 0)){
		fprintf(stream, "%s\n", buf);
	}
	mib[2] = GRP_ENDGRENT;
	sysctl(mib, 3, NULL, NULL, NULL, 0);
}
void 
mk_passwd(FILE *stream)
{
	int mib[4] = {CTL_USER, USER_PWD, PWD_GETPWUID};
	char buf[WIN_PATH_MAX];
	size_t size = WIN_PATH_MAX;

	mib[3] = ROOT_UID;
	if (!sysctl(mib, 4, buf, &size, NULL, 0)){
		fprintf(stream, "%s\n", buf);
	}
	mib[3] = DAEMON_UID;
	if (!sysctl(mib, 4, buf, &size, NULL, 0)){
		fprintf(stream, "%s\n", buf);
	}
	mib[3] = ROOT_GID;
	if (!sysctl(mib, 4, buf, &size, NULL, 0)){
		fprintf(stream, "%s\n", buf);
	}
	mib[3] = DOMAIN_NT_SERVICE_RID_INSTALLER;	/* Vista */
	if (!sysctl(mib, 4, buf, &size, NULL, 0)){
		fprintf(stream, "%s\n", buf);
	}
	mib[2] = PWD_GETPWENT;
	while (!sysctl(mib, 3, buf, &size, NULL, 0)){
		fprintf(stream, "%s\n", buf);
	}
	mib[2] = PWD_ENDPWENT;
	sysctl(mib, 3, NULL, NULL, NULL, 0);
}
void 
mk_resolv(FILE *stream)
{
	LONG lSize = 0;
	DWORD dwStatus;
	FIXED_INFO *fInfo;
	DWORD dwIndex;
	IP_ADDR_STRING *ipEntry;

	dwStatus = GetNetworkParams(NULL, &lSize);
	if (lSize > 0){
		fInfo = win_malloc(lSize);
		GetNetworkParams(fInfo, &lSize);
		fprintf(stream, "domain %s\n", fInfo->DomainName);
		for (ipEntry = &fInfo->DnsServerList; ipEntry; ipEntry = ipEntry->Next){
			fprintf(stream, "nameserver %s\n", ipEntry->IpAddress.String);
		}
		win_free(fInfo);
	}else{
		WIN_ERR("GetNetworkParams(): %s\n", win_strerror(dwStatus));
	}
}
int 
mk_fsent(WIN_CFDATA *Config, WIN_CFDRIVER *Driver, struct statfs *info)
{
	int result = 0;
	WIN_STATFS fsInfo = {0};

	if (vfs_getfsstat(Config, Driver, &fsInfo)){
		statfs_posix(info, &fsInfo);
	}else{
		result = -1;
	}
	return(result);
}
int 
mk_fstab(FILE *stream)
{
	int result = 0;
	WIN_CFDATA cfData;
	DWORD dwFlags = WIN_MNT_NOWAIT;
	WIN_CFDRIVER cfDriver;
	struct statfs info;

	printf("/dev/root\t/\tffs\trw\t0\t0\n");
	if (!vfs_setvfs(&cfData, dwFlags)){
		fprintf(stderr, "vfs_setvfs(): %s\n", win_strerror(errno_win()));
	}else while (vfs_getvfs(&cfData, dwFlags)){
		if (cfData.FSType == FS_TYPE_DRIVE){
			drive_statvfs(&cfData, dwFlags, &cfDriver);
			drive_match(cfData.NtName, cfData.DeviceType, &cfDriver);
			if (!mk_fsent(&cfData, &cfDriver, &info)){
				print_fsent(&cfData, &info);
			}
		}else if (cfData.FSType == FS_TYPE_PDO){
			pdo_statvfs(&cfData, dwFlags, &cfDriver);
			pdo_match(cfData.NtName, cfData.DeviceType, &cfDriver);
		}
	}
	vfs_endvfs(&cfData);
	return(result);
}
void 
mk_vfsent(WIN_FS_TYPE Type)
{
	WIN_CFDATA fsEnum;
	char buf[PATH_MAX] = "";

	if (!vfs_setvfs(&fsEnum, 0)){
		fprintf(stderr, "vfs_setfsstat(): %s\n", strerror(errno));
	}else while (vfs_getvfs(&fsEnum, 0)){
		if (fsEnum.FSType == Type){
			if (_verbose){
				printf("%ls: %ls\n", fsEnum.DosPath, fsEnum.NtPath);
			}else{
				printf("%ls: %ls\n", fsEnum.BusName, fsEnum.NtPath);
			}
		}
	}
	vfs_endvfs(&fsEnum);
}
void 
mk_vol(FILE *stream)
{
	WIN_CFDATA cfData;
	CHAR szMessage[MAX_MESSAGE];
	char buf[PATH_MAX];

	if (!vfs_setvfs(&cfData, 0)){
		fprintf(stderr, "vfs_setfsstat(): %s\n", strerror(errno));
	}else while (vfs_getvfs(&cfData, 0)){
		if (cfData.FSType == FS_TYPE_VOLUME){
			printf("%ls: %ls\n", cfData.DosPath, cfData.NtPath);
			if (!vol_stat(cfData.DosPath, szMessage)){
				printf("   %s\n", win_strerror(errno_win()));
			}else{
				printf("   %s\n", szMessage);
			}
		}
	}
	vfs_endvfs(&cfData);
}
void 
mk_ifent(WIN_FS_TYPE Type)
{
	WIN_IFDATA ifData;
	WIN_CFDRIVER ifDriver;

	if (!ws2_setvfs(&ifData)){
		fprintf(stderr, "ws2_setvfs(): %s\n", strerror(errno));
	}else while (ws2_getvfs(&ifData, &ifDriver)){
		if (ifData.FSType == Type){
			printf("%ls: Index(%d) Type(%d): %ls\n", ifData.NtName, ifData.Index, ifData.Type, ifDriver.Comment);
		}
	}
	ws2_endvfs(&ifData);
}

/****************************************************/

int 
main(int argc, char *argv[])
{
	char *cmd = NULL;
	char *token;
	char *progname = *argv++;
	int result = 0;

	while (token = *argv){
		if (!strcmp(token, "-v")){
			_verbose++;
		}else if (!strcmp(token, "-p")){
			_paths++;
		}else if (token[0] != '-'){
			cmd = token;
		}
		argv++;
	}
	if (!cmd){
		print_usage(progname);

	}else if (!strcmp(cmd, "group")){
		mk_group(stdout);

	}else if (!strcmp(cmd, "passwd")){
		mk_passwd(stdout);

	}else if (!strcmp(cmd, "fstab")){
		mk_fstab(stdout);

	}else if (!strcmp(cmd, "resolv")){
		mk_resolv(stdout);

	}else if (!strcmp(cmd, "pdo")){
		mk_vfsent(FS_TYPE_PDO);

	}else if (!strcmp(cmd, "vol")){
		mk_vol(stdout);

	}else if (!strcmp(cmd, "fdo")){
		mk_vfsent(FS_TYPE_PROCESS);

	}else if (!strcmp(cmd, "link")){
		mk_vfsent(FS_TYPE_LINK);

	}else if (!strcmp(cmd, "drive")){
		mk_vfsent(FS_TYPE_DRIVE);

	}else if (!strcmp(cmd, "npf")){
		mk_vfsent(FS_TYPE_NPF);

	}else if (!strcmp(cmd, "if")){
		mk_ifent(FS_TYPE_WINSOCK);

	}else if (!strcmp(cmd, "ndis")){
		mk_ifent(FS_TYPE_NDIS);

	}else{
		printf("%s: no such entity.\n", cmd);
		result = -1;

	}
	return(result);
}
