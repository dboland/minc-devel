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

#include <sys/param.h>

#define CRONTAB_GID	500000003
#define AUTH_GID	500000004
#define DAEMON_UID	500000006
#define TERM_GID	500000013
#define ROOT_UID	500000018
#define DAEMON_GID	500000019
#define WHEEL_GID	532000544
#define BIN_GID		532000545
#define OPERATOR_GID	532000551
#define KMEM_GID	WHEEL_GID
#define NOBODY_UID	DAEMON_UID

/* sys/fcntl.c */

#define O_NOCROSS		0x00001000
#define O_NOSLASH		0x00002000
#define O_SYMLINK		0x00004000
#define O_INODE			0x00008000

#define AT_NOCROSS		0x0010
#define AT_NOSLASH		0x0020
#define AT_SYMLINK		0x0040
#define AT_INODE		0x0080
#define AT_LOCKLEAF		0x0100

/* machine/param.h */

#define MSGBUFSIZE		(4 * PAGE_SIZE)

/* sys/signal.c */

typedef void (*atexit_t)(void);

typedef struct {
	int c_dx;
	int c_ax;
	int c_di;
	int c_si;
	int c_bx;
	int c_cx;
	void *Task;
	unsigned long Code;
	unsigned long Base;	/* return address */
} call_t;

typedef struct {
	int argc;
	char **argv;
	char **env;
	void *frame;
	char *reserved1;
	void *text;
	void *data;
	void *bss;
	void *end;
} exec_t;
