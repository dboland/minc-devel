/*	$OpenBSD: src/sys/sys/systm.h,v 1.100 2013/06/11 18:15:54 deraadt Exp $	*/
/*	$NetBSD: systm.h,v 1.50 1996/06/09 04:55:09 briggs Exp $	*/

/*-
 * Copyright (c) 1982, 1988, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)systm.h	8.4 (Berkeley) 2/23/94
 */

#ifndef __SYSTM_H__
#define __SYSTM_H__

extern struct sysent {		/* system call table */
	short	sy_narg;	/* number of args */
	short	sy_argsize;	/* total size of arguments */
	int	sy_flags;
	void	*sy_call;	/* implementing function */
} sysent[];

#define SY_MPSAFE               0x01
#define SY_NOLOCK               0x02

#define sys___tfork		sys_nosys
#define sys___thrsleep		sys_nosys
#define sys___thrwakeup		sys_nosys
#define sys___thrsigdivert	sys_nosys
#define sys___set_tcb		sys_nosys
#define sys___get_tcb		sys_nosys

#define sys_sched_yield		sys_nosys
#define sys_quotactl		sys_nosys
#define sys_getitimer		sys_nosys
#define sys_kevent		sys_nosys
#define sys_mincore		sys_nosys
#define sys_clock_settime	sys_nosys
#define sys_clock_getres	sys_nosys
#define sys_sigreturn		sys_nosys
#define sys_adjtime		sys_nosys
#define sys_adjfreq		sys_nosys
#define sys_sysarch		sys_nosys
#define sys_mlock		sys_nosys
#define sys_munlock		sys_nosys
#define sys_mquery		sys_nosys
#define sys_minherit		sys_nosys
#define sys_msync		sys_nosys
#define sys_mlockall		sys_nosys
#define sys_munlockall		sys_nosys
#define sys_getfh		sys_nosys
#define sys_fhstatfs		sys_nosys
#define sys_fhstat		sys_nosys
#define sys_fhopen		sys_nosys
#define sys_preadv		sys_nosys
#define sys_pwritev		sys_nosys
#define sys_kqueue		sys_nosys
#define sys_sigaltstack		sys_nosys
#define sys_setrtable		sys_nosys

void	consinit(void);		/* sys/arch/i386/i386/machdep.c:3392 */

void    cpu_startup(void);	/* sys/arch/i386/i386/machdep.c:382 */
void    cpu_configure(void);	/* sys/arch/i386/i386/autoconf.c:105 (configure devices) */
void 	diskconf(char *);	/* sys/arch/i386/i386/autoconf.c:175 (configure root/swap) */

#endif /* __SYSTM_H__ */
