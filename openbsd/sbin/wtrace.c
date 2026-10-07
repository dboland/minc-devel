#include <sys/fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <errno.h>

#include "win/windows.h"
#include "win/winsock2.h"
#include "win/iphlpapi.h"
#include "win_posix.h"
#include "vfs_posix.h"
#include "ws2_posix.h"
#include "bsd_posix.h"
#include "arch_posix.h"

#include "../libtrace/libtrace.h"

int _verbose;

/************************************************************/

void 
usage(void)
{
	printf("\nPerform trace on various Windows objects.\n\n");
	printf("Usage: %s [options]\n", __progname);
	printf("\nOptions\n");
	printf(" -C ACCOUNT\t\t%s\n", "print capabilities of ACCOUNT");
	printf(" -I\t\t\t%s\n", "print processor Information");
	printf(" -P\t\t\t%s\n", "print Process token");
	printf(" -T\t\t\t%s\n", "print Thread token (if any)");
	printf(" -M DRIVE\t\t%s\n", "print mount info of DRIVE letter");
	printf(" -d\t\t\t%s\n", "print ACL Desktop");
	printf(" -s\t\t\t%s\n", "print ACL Window Station");
	printf(" -f PATH\t\t%s\n", "print ACL on PATH");
	printf(" -p\t\t\t%s\n", "print ACL Process");
	printf(" -v, --verbose\t\tbe verbose if applicable\n");
	printf(" -t TABLE\t\tshow TABLE (see below)\n");
	printf("\nTables\n");
	printf(" pdo\t\t\tWindows PDO device table (physical drivers)\n");
	printf(" fdo\t\t\tWindows FDO device table (function drivers)\n");
	printf(" vol\t\t\tWindows volume table\n");
	printf(" link\t\t\tWindows link table\n");
	printf(" drive\t\t\tWindows drive table\n");
	printf(" npf\t\t\tNetGroup Packet Filter table\n");
//	printf(" ndis\t\t\tWindows network driver table\n");
	printf(" if\t\t\tWindows network adapter table\n");
}
void 
wtrace_vfsent(WIN_FS_TYPE Type)
{
	WIN_CFDATA fsEnum;
	char buf[PATH_MAX] = "";

	if (!vfs_setconf(&fsEnum, 0)){
		fprintf(stderr, "vfs_setfsstat(): %s\n", strerror(errno));
	}else while (vfs_getconf(&fsEnum, 0)){
		if (fsEnum.FSType == Type){
			if (_verbose){
				printf("%ls: %ls\n", fsEnum.DosPath, fsEnum.NtPath);
			}else{
				printf("%ls: %ls\n", fsEnum.BusName, fsEnum.NtPath);
			}
		}
	}
	vfs_endconf(&fsEnum);
}
void 
wtrace_vol(FILE *stream)
{
	WIN_CFDATA cfData;
	CHAR szMessage[MAX_MESSAGE];
	char buf[PATH_MAX];

	if (!vfs_setconf(&cfData, 0)){
		fprintf(stderr, "vfs_setfsstat(): %s\n", strerror(errno));
	}else while (vfs_getconf(&cfData, 0)){
		if (cfData.FSType == FS_TYPE_VOLUME){
			printf("%ls: %ls\n", cfData.DosPath, cfData.NtPath);
			if (!vol_lookup(cfData.DosPath, szMessage)){
				printf("   %s\n", win_strerror(errno_win()));
			}else{
				printf("   %s\n", szMessage);
			}
		}
	}
	vfs_endconf(&cfData);
}
void 
wtrace_ifent(WIN_FS_TYPE Type)
{
	WIN_IFDATA ifData;
	WIN_IFDRIVER ifDriver;

	if (!ws2_setconf(&ifData, WS_AF_UNSPEC)){
		fprintf(stderr, "ws2_setconf(): %s\n", strerror(errno));
	}else while (ws2_getconf(&ifData, &ifDriver)){
		printf("%ls: Index(%d) Type(%d): %ls\n", 
			ifDriver.AdapterName, ifDriver.IfIndex, ifDriver.IfType, ifDriver.FriendlyName);
	}
	ws2_endconf(&ifData);
}
void 
wtrace_table(char *table)
{
	if (!table){
		usage();

	}else if (!strcmp(table, "pdo")){
		wtrace_vfsent(FS_TYPE_PDO);

	}else if (!strcmp(table, "vol")){
		wtrace_vol(stdout);

	}else if (!strcmp(table, "fdo")){
		wtrace_vfsent(FS_TYPE_PROCESS);

	}else if (!strcmp(table, "link")){
		wtrace_vfsent(FS_TYPE_LINK);

	}else if (!strcmp(table, "drive")){
		wtrace_vfsent(FS_TYPE_DRIVE);

	}else if (!strcmp(table, "npf")){
		wtrace_vfsent(FS_TYPE_NPF);

	}else if (!strcmp(table, "if")){
		wtrace_ifent(FS_TYPE_WINSOCK);

//	}else if (!strcmp(table, "ndis")){
//		wtrace_ifent(FS_TYPE_NDIS);

	}else{
		printf("%s: No such table.\n", table);

	}
}

/****************************************************/

int 
main(int argc, char* argv[])
{
	WIN_NAMEIDATA wPath = {0};
	char *prog = *argv++;
	char *opt = NULL;
	char *token = "";
	char *arg;

	while (arg = *argv++){
		if (!strcmp(arg, "-v")){
			_verbose++;
		}else if (arg[0] == '-'){
			opt = arg;
		}else{
			token = arg;
		}
	}
	if (!opt){
		usage();

	}else switch(opt[1]){
		case 'd':
			win_ktrace(STRUCT_ACL_DESKTOP, 0x1000, token);
			break;
		case 's':
			win_ktrace(STRUCT_ACL_STATION, 0x2000, token);
			break;
		case 'f':
			win_ktrace(STRUCT_ACL_FILE, 0x1000, path_win(&wPath, token, O_NOFOLLOW)->Resolved);
			break;
		case 'o':
			win_ktrace(STRUCT_ACL_OBJECT, 0x1000, token);
			break;
		case 'p':
			win_ktrace(STRUCT_ACL_PROCESS, 0x0800, token);
			break;
//		case '?':
//			win_ktrace(STRUCT_NAMEI, 0x1000, path_win(&wPath, token, O_NOFOLLOW));
//			break;
		case 'I':
			win_ktrace(STRUCT_SYSTEM_INFO, 0x0800, token);
			break;
		case 'P':
			win_ktrace(STRUCT_TOKEN_PROCESS, 0x1000, token);
			break;
		case 'T':
			win_ktrace(STRUCT_TOKEN_THREAD, 0x1000, token);
			break;
		case 'C':
			win_ktrace(STRUCT_SID_RIGHTS, 0x0800, token);
			break;
		case 'M':
			win_ktrace(STRUCT_MOUNT, 0x800, token);
			break;
		case 't':
			wtrace_table(token);
			break;
		default:
			printf("%s: No such option.\n", opt);
	}
}
