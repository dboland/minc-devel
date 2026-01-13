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

#include <sys/ioctl.h>
#include <sys/param.h>	/* NAME_MAX */
#include <sys/mount.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pwd.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <paths.h>
#include <ttyent.h>
#include <regex.h>
#include <fstab.h>

char 	_term[NAME_MAX] = "interix";
char 	_home;

/************************************************************/

void 
die(const char *msg, ...)
{
	va_list args;

	va_start(args, msg);
	vfprintf(stderr, msg, args);
	va_end(args);
	exit(-1);
}
void 
args(int argc, char *argv[])
{
	int ch;

	while ((ch = getopt(argc, argv, "h")) > 0){
		switch (ch){
		case 'h':
			_home++;
			break;
		default:
			die("Usage: %s [-h]\n", __progname);
		}
	}
}
int 
shell(void)
{
	const struct passwd *pwd;
	char *args[] = {"/bin/ksh", "-l", NULL};
	uid_t uid = getuid();
	char *home = "/";
	char *sh;

	if (pwd = getpwuid(uid)){
		setenv("USER", pwd->pw_name, 1);
		setenv("HOME", pwd->pw_dir, 1);
		home = pwd->pw_dir;
		args[0] = pwd->pw_shell;
	}else{
		fprintf(stderr, "getpwuid(%d): %s\n", uid, strerror(errno));
	}
	if (_home && chdir(home) < 0)
		fprintf(stderr, "chdir(%s): %s\n", home, strerror(errno));
	execve(*args, args, environ);
	fprintf(stderr, "execve(%s): %s\n", *args, strerror(errno));
	return(-1);
}
int
getty(const char *name)
{
	int result = -1;
	int fd;
	struct ttyent *term;
	char path[PATH_MAX] = _PATH_DEV;

	strcat(path, name);
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
	while (term = getttyent()){
		if (!strcmp(term->ty_name, name)){
			strcpy(_term, term->ty_type);
		}
	}
	endttyent();
	setenv("TERM", _term, 1);
	return(result);
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

/************************************************************/

int 
main(int argc, char *argv[], char *envp[])
{
	int result = 0;
	pid_t pid = getpid();

	args(argc, argv);
	if (pid > 1){
		setsid();
		getty("ptm");
	}else{
		fs_mount();
		getty("console");
	}
	shell();
}
