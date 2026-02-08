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

#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/param.h>	/* MAXCOMLEN */
#include <sys/uio.h>
#include <sys/ktrace.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pwd.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <signal.h>
#include <unistd.h>
#include <util.h>
#include <poll.h>
#include <paths.h>

#include "win/windows.h"

#define KTR_DEFAULT (KTRFAC_SYSCALL | KTRFAC_SYSRET | KTRFAC_NAMEI | \
	KTRFAC_PSIG | KTRFAC_EMUL | KTRFAC_STRUCT | KTRFAC_INHERIT)

int 	_verbose;
int	_quitting;
char	_terminal[64] = "xterm";
char	_home;

#define BACKGROUND_BLACK	0
#define FOREGROUND_WHITE	(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)

/************************************************************/

int 
x_init(VOID)
{
	int result = 0;
	DWORD dwMode[2];
	HANDLE hInput, hScreen;

	hInput = GetStdHandle(STD_INPUT_HANDLE);
	hScreen = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleMode(hInput, &dwMode[0]);
	GetConsoleMode(hScreen, &dwMode[1]);
	dwMode[0] &= ~(ENABLE_PROCESSED_INPUT | ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
	dwMode[1] |= DISABLE_NEWLINE_AUTO_RETURN;
	if (!SetConsoleTextAttribute(hScreen, BACKGROUND_BLACK | FOREGROUND_WHITE)){
		result = 1;
	}else if (!SetConsoleOutputCP(CP_UTF8)){
		result = 2;
	}else if (!SetConsoleMode(hInput, dwMode[0] | ENABLE_VIRTUAL_TERMINAL_INPUT)){
		result = 3;
	}else if (!SetConsoleMode(hScreen, dwMode[1] | ENABLE_VIRTUAL_TERMINAL_PROCESSING)){
		result = 4;
	}
	return(result);
}

/************************************************************/

/* In a real terminal emulation, we would not be reading from STDIN, but
 * directly from a window manager, processing window events and translating
 * them to ANSI encoded input as if coming from a terminal.
 * For details on implementation, see: ./libposix/vfs/char/char_input.c
 */

int 
write_input(int master)
{
	char buf[MAX_INPUT];
	ssize_t count = 0;

	count = read(STDIN_FILENO, buf, MAX_INPUT);
	if (_verbose)
		fprintf(stderr, "[in:%d]", count);
	if (count > 0)
		write(master, buf, count);
	return(count);
}

/* In a real terminal emulation, we would not be writing to STDOUT, but
 * we would be reading the ANSI encoded output from the remote program and
 * translate it into drawing operations for the application window.
 * For details on implementation, see: ./libposix/vfs/char/char_screen.c
 */

int 
write_output(int master)
{
	char buf[MAX_INPUT];
	ssize_t count = 0;

	count = read(master, buf, MAX_INPUT);
	if (_verbose)
		fprintf(stderr, "[out:%d]", count);
	if (count > 0)
		write(STDOUT_FILENO, buf, count);
	return(count);
}

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
sig(int signum)
{
	switch (signum){
		case SIGCHLD:
			_quitting++;
			break;
		default:
			printf("%s: %s\n", __progname, strsignal(signum));
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

	if (sh = getenv("SHELL"))
		args[0] = sh;

	if (_home && chdir(home) < 0)
		fprintf(stderr, "chdir(%s): %s\n", home, strerror(errno));

	execve(*args, args, environ);
	die("execve(%s): %s\n", *args, strerror(errno));
}
int 
multiplex(int master)
{
	struct pollfd pfds[2] = {0};
	int result = -1;

	pfds[0].fd = STDIN_FILENO;
	pfds[0].events = POLLIN;
	pfds[1].fd = master;
	pfds[1].events = POLLIN;
	while (!_quitting){
		if (_verbose)
			fprintf(stderr, "[poll]");
		if (poll(pfds, 2, INFTIM) < 0)
			break;
		if (pfds[0].revents)
			result = write_input(master);
		if (pfds[1].revents)
			result = write_output(master);
	}
	return(result);
}
pid_t 
getty(void)
{
	int result = -1;
	int pid;
	int master, slave;
	int status;
	struct winsize ws = {0};

	/* lib/libutil/pty.c */
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) < 0)
		die("ioctl(TIOCGWINSZ): %s\n", strerror(errno));
	else if (openpty(&master, &slave, NULL, NULL, &ws) < 0)
		die("openpty(): %s\n", strerror(errno));
	switch (pid = fork()){
		case -1:
			die("fork(): %s\n", strerror(errno));
		case 0:
			setsid();
			if (ioctl(slave, TIOCSCTTY, NULL) < 0)
				die("ioctl(TIOCSCTTY): %s\n", strerror(errno));
			close(master);
			dup2(slave, STDIN_FILENO);
			dup2(slave, STDOUT_FILENO);
			dup2(slave, STDERR_FILENO);
			close(slave);
			setenv("TERM", _terminal, 0);
			shell();
		default:
			close(slave);
			result = multiplex(master);
			waitpid(pid, &status, 0);
			close(master);
			result = 0;
	}
	return(result);
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

/************************************************************/

int 
main(int argc, char *argv[], char *envp[])
{
	int result = 0;
	char *prog = *argv++;
	char *token;

	if (x_init() > 0){
		fprintf(stderr, "Console has no xterm capability.\n");
	}
	while (token = *argv++){
		if (!strcmp(token, "-v")){
			_verbose++;
		}else if (!strcmp(token, "-h")){
			_home++;
		}
	}
	signal(SIGINT, sig);
	signal(SIGQUIT, sig);
	signal(SIGCHLD, sig);
	result = getty();
	return(result);
}
