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
#include <pwd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <paths.h>
#include <fstab.h>
#include <regex.h>
#include <locale.h>
#include <dirent.h>
#include <ttyent.h>

#include <sys/stdarg.h>
#include <sys/fcntl.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/sysctl.h>
#include <sys/uio.h>
#include <sys/ktrace.h>
#include <sys/reboot.h>
#include <sys/socket.h>
#include <sys/tty.h>
#include <sys/termios.h>

extern char *__progname;
extern char **environ;

#include "../../../../libposix/kern/systm.h"

#define _KERNEL
#include <net/if.h>

#define KTR_DEFAULT (KTRFAC_SYSCALL | KTRFAC_SYSRET | KTRFAC_NAMEI | \
	KTRFAC_PSIG | KTRFAC_EMUL | KTRFAC_STRUCT | KTRFAC_INHERIT)

char *_argv[] = {"/sbin/console", NULL, NULL, NULL};
char **_args = _argv;
char *_lctype = "en_US.UTF-8";

/************************************************************/

void 
sig(int signum)
{
	if (signum == SIGQUIT){
		fprintf(stderr, "System halt.\n");
	}else if (signum == SIGHUP){
		fprintf(stderr, "System reboot.\n");
	}else{
		fprintf(stderr, "%s: %s\n", __progname, strsignal(signum));
	}
}
void 
fs_mount(void)
{
	int result = 0;
	struct fstab *tab;
	char fspec[PATH_MAX];
	struct ntfs_args args = {fspec, {0}, getuid(), getgid(), 00755, 0};
	regex_t rebuf;

	/* Yes, those are word boundaries */
	regcomp(&rebuf, "[[:<:]]noauto[[:>:]]", REG_EXTENDED);
	while (tab = getfsent()){
		if (regexec(&rebuf, tab->fs_mntops, 0, NULL, REG_EXTENDED)){
			strcpy(fspec, tab->fs_spec);
			mount(tab->fs_vfstype, tab->fs_file, 0, &args);
		}
	}
	endfsent();
	regfree(&rebuf);
}
void 
fs_unmount(void)
{
	int result = 0;
	struct fstab *tab;

	while (tab = getfsent()){
		unmount(tab->fs_file, MNT_FORCE);
	}
	endfsent();
}
int 
trpoints(const char *opts)
{
	int result = KTR_DEFAULT;

	if (!opts)
		return(result);
	if (strchr(opts, 'u'))
		result |= KTRFAC_USER;
	if (strchr(opts, 'i'))
		result |= KTRFAC_GENIO;
	return(result);
}
void 
args(int argc, char *argv[])
{
	char *token;
	char *prog = *argv++;

	while (token = *argv){
		if (token[0] != '-'){
			_args = argv;
			break;
		}else if (token[1] == 'h'){
			_args[1] = "-h";
		}else if (token[1] == 't'){
			ktrace("minc.out", KTROP_SET, KTR_DEFAULT, 0);
		}
		argv++;
	}
}

/************************************************************/

int 
main(int argc, char *argv[], char *env[])
{
	int result = -1;
	pid_t pid = getpid();
	int status;
	char root[260];

	args(argc, argv);
	diskconf(root);
	setenv("MINCROOT", root, 1);
	setenv("PATH", _PATH_DEFPATH, 1);
	setenv("TMP", "/tmp", 1);	/* GNU CC */
	setenv("LC_CTYPE", _lctype, 0);
	if (!pid){
		signal(SIGQUIT, sig);
		signal(SIGHUP, sig);
		consinit();
		cpu_configure();
		ifinit();
		fs_unmount();
		fprintf(stderr, "\r\n");
	}
	switch (pid = fork()){
		case -1:
			fprintf(stderr, "fork(): %s\n", *args, strerror(errno));
			break;
		case 0:
			execvp(*_args, _args);
			fprintf(stderr, "execvp(%s): %s\n", *_args, strerror(errno));
			break;
		default:
			waitpid(pid, &status, 0);
			result = 0;
	}
	return(result);
}
