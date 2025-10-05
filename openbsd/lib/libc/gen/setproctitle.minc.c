/*      $OpenBSD: src/lib/libc/gen/setproctitle.c,v 1.12 2011/05/30 18:48:33 martynas Exp $ */
/*
 * Copyright (c) 1994, 1995 Christopher G. Demetriou
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Christopher G. Demetriou
 *      for the NetBSD Project.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "win/windows.h"
#include "win_posix.h"
#include "vfs_posix.h"

/* Original OpenBSD source has some issues to be resolved:
 * 1. It uses the _ps_strings struct in uvm_param.h, which is not #included.
 * 2. It fetches VM_PSSTRINGS, which contains the program's argv[] and 
 * environ[]. It then clobbers the struct: argc is now 1 and *argv is 
 * assigned to a static title buffer.
 */

void 
setproctitle(const char *fmt, ...)
{
	extern char *__progname;
	va_list args;
	char buf[WIN_MAX_PROCTITLE];
	int used;

	va_start(args, fmt);
	if (fmt){
		used = snprintf(buf, WIN_MAX_PROCTITLE, "%s: ", __progname);
		if (used >= WIN_MAX_PROCTITLE){
			used = WIN_MAX_PROCTITLE - 1;
		}else if (used < 0){
			used = 0;
		}
		vsnprintf(buf + used, WIN_MAX_PROCTITLE - used, fmt, args);
	}else{
		snprintf(buf, WIN_MAX_PROCTITLE, "%s", __progname);
	}
	va_end(args);
	vfs_setproctitle(buf);
}
