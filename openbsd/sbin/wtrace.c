#include <sys/fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

#include "win/windows.h"
#include "win_posix.h"
#include "vfs_posix.h"
#include "bsd_posix.h"

#include "../libtrace/libtrace.h"

/************************************************************/

void 
usage(void)
{
	printf("\nPerform trace on various Windows objects.\n\n");
	printf("Usage: %s [options]\n", __progname);
	printf("\nOptions\n");
	printf(" -D\t\t\t%s\n", "print ACL Desktop");
	printf(" -I\t\t\t%s\n", "print processor Information");
	printf(" -P\t\t\t%s\n", "print Process token");
	printf(" -S\t\t\t%s\n", "print ACL Window Station");
	printf(" -T\t\t\t%s\n", "print Thread token (if any)");
	printf(" -c ACCOUNT\t\t%s\n", "print capabilities of ACCOUNT");
	printf(" -f PATH\t\t%s\n", "print ACL file/directory");
	printf(" -m DRIVE\t\t%s\n", "print Mount info of DRIVE letter");
	printf(" -p\t\t\t%s\n", "print ACL Process");
}
int 
main(int argc, char* argv[])
{
	WIN_NAMEIDATA wPath = {0};
	char *prog = *argv++;
	char *token = "";
	char *cmd = NULL;
	char *arg = NULL;

	while (arg = *argv++){
		if (arg[0] == '-'){
			cmd = arg;
		}else{
			token = arg;
		}
	}
	if (!cmd)
		usage();

	else switch(cmd[1]){
		case 'D':
			win_ktrace(STRUCT_ACL_DESKTOP, 0x1000, token);
			break;
		case 'I':
			win_ktrace(STRUCT_SYSTEM_INFO, 0x0800, token);
			break;
		case 'P':
			win_ktrace(STRUCT_TOKEN_PROCESS, 0x1000, token);
			break;
		case 'S':
			win_ktrace(STRUCT_ACL_STATION, 0x2000, token);
			break;
		case 'T':
			win_ktrace(STRUCT_TOKEN_THREAD, 0x1000, token);
			break;
		case 'c':
			win_ktrace(STRUCT_SID_RIGHTS, 0x0800, token);
			break;
		case 'f':
			win_ktrace(STRUCT_ACL_FILE, 0x1000, path_win(&wPath, token, O_NOFOLLOW)->Resolved);
			break;
		case 'm':
			win_ktrace(STRUCT_MOUNT, 0x800, token);
			break;
		case 'o':
			win_ktrace(STRUCT_ACL_OBJECT, 0x1000, token);
			break;
		case 'p':
			win_ktrace(STRUCT_ACL_PROCESS, 0x0800, token);
			break;
		case 's':
			win_ktrace(STRUCT_NAMEI, 0x1000, path_win(&wPath, token, O_NOFOLLOW));
			break;
		default:
			printf("%s: No such option.\n", cmd);
	}
}
