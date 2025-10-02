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

/* sys/fcntl.c */

#define O_NOCROSS		0x00001000
#define O_NOSLASH		0x00002000
#define O_SYMLINK		0x00004000
#define O_OBJECT		0x00008000

#define AT_NOCROSS		0x0010
#define AT_NOSLASH		0x0020
#define AT_SYMLINK		0x0040
#define AT_OBJECT		0x0080
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
