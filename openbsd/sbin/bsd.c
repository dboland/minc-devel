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

#include "../../libposix/kern/systm.h"

#define _KERNEL
#include <net/if.h>

#define KTR_DEFAULT (KTRFAC_SYSCALL | KTRFAC_SYSRET | KTRFAC_NAMEI | \
	KTRFAC_PSIG | KTRFAC_EMUL | KTRFAC_STRUCT | KTRFAC_INHERIT)

unsigned char		_verbose;
unsigned char		_boot;
unsigned char		_home;

/* src/sys/netinet/ip_input.c */
void tty_init(void);

/* sys/arch/i386/stand/boot/crt0.c */

/************************************************************/

void 
die(const char *msg, ...)
{
	va_list args;

	va_start(args, msg);
	vfprintf(stderr, msg, args);
	va_end(args);
	_exit(-1);
}
void 
sig(int signum)
{
	if (signum == SIGQUIT){
		fprintf(stderr, "System halt.\n");
		kill(1, SIGKILL);
	}else if (signum == SIGHUP){
		fprintf(stderr, "System reboot.\n");
		kill(1, SIGHUP);
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
getty(const char *path)
{
	int result = -1;
	int fd;

	/* copy of ./lib/libutil/login_tty.c
	 */
	fd = open(path, O_RDWR);
	if (fd < 0){
		fprintf(stderr, "open(%s): %s\n", path, strerror(errno));
	}else if (ioctl(fd, TIOCSCTTY, NULL) < 0){
		fprintf(stderr, "ioctl(TIOCSCTTY): %s\n", strerror(errno));
	}else{
		dup2(fd, STDIN_FILENO);
		dup2(fd, STDOUT_FILENO);
		dup2(fd, STDERR_FILENO);
		if (fd > STDERR_FILENO)
			close(fd);
		result = 0;
	}
	return(result);
}
int 
shell(char *args[])
{
	const struct passwd *pwd = NULL;
	char *login = getlogin();
	char *home = "/";

	if (pwd = getpwnam(login)){
		setenv("USER", pwd->pw_name, 1);
		setenv("HOME", pwd->pw_dir, 1);
		home = pwd->pw_dir;
	}else{
		fprintf(stderr, "getpwnam(%s): %s\n", login, strerror(errno));
	}
	if (_home && chdir(home) < 0){
		fprintf(stderr, "chdir(%s): %s\n", home, strerror(errno));
	}
	execve(*args, args, environ);
	fprintf(stderr, "execve(%s): %s\n", *args, strerror(errno));
	return(0);
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
	int ch;

	while ((ch = getopt(argc, argv, "kvbh")) > 0){
		switch (ch){
		case 'v':
			_verbose++;
			break;
		case 'b':
			_boot++;
			break;
		case 'k':
			ktrace("boot.out", KTROP_SET, trpoints(optarg), 0);
			break;
		case 'h':
			_home++;
			break;
		}
	}
}
void 
boot(void)
{
	char *args[] = {"/sbin/init", NULL};

	consinit();
	tty_init();
	ifinit();
	cpu_configure();
	fs_unmount();		// fsck.exe operation?
	fprintf(stderr, "\r\n");
	execve(*args, args, environ);
	fprintf(stderr, "execve(%s): %s\n", *args, strerror(errno));
}
void 
single(void)
{
	int mib[2] = {CTL_KERN, KERN_SECURELVL};
	int level = 1;
	char *args[] = {"/bin/ksh", "-l", NULL};

	consinit();
	tty_init();
	ifinit();
	cpu_configure();
	fs_unmount();
	fs_mount();
	sysctl(mib, 2, NULL, NULL, &level, sizeof(int));
//	close(0);
//	close(1);
//	close(2);
	(void) revoke(_PATH_CONSOLE);
	setsid();
	getty(_PATH_CONSOLE);
	shell(args);
}
void 
multi(void)
{
	char *args[] = {"/bin/ksh", "-l", NULL};

	setsid();
	getty(PATH_PTMDEV);
	shell(args);
}
void 
state(int level)
{
	setenv("TERM", "interix", 0);
	setenv("LC_CTYPE", "en_US.UTF-8", 0);
	if (_boot)
		boot();
	else if (!level)
		single();
	else
		multi();
}
void 
halt(int level)
{
	if (!level)
		fs_unmount();
}

/************************************************************/

int 
main(int argc, char *argv[], char *envp[])
{
	int mib[2] = {CTL_KERN, KERN_SECURELVL};
	size_t size = sizeof(int);
	int level = 0;
	pid_t pid;
	int status;
	char root[260];

	diskconf(root);
	setenv("MINCROOT", root, 1);
	setenv("PATH", _PATH_DEFPATH, 1);
	setenv("TMP", "/tmp", 1);	/* GNU CC */
	args(argc, argv);
	sysctl(mib, 2, &level, &size, NULL, 0);
	switch (pid = fork()){
		case -1:
			die("fork(): %s\n", strerror(errno));
		case 0:
			state(level);
			break;
		default:
			waitpid(pid, &status, 0);
			halt(level);
	}
	return(0);
}
