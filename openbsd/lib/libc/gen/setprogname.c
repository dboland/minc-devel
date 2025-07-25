/* $OpenBSD: src/lib/libc/gen/setprogname.c,v 1.4 2013/11/12 06:09:48 deraadt Exp $ */
/*
 * Copyright (c) 2013 Antoine Jacoutot <ajacoutot@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <string.h>
#include <stdlib.h>

//extern const char *__progname;

void
setprogname(const char *progname)
{
	const char *tmpn;

	tmpn = strrchr(progname, '/');
	if (tmpn == NULL)
		__progname = (char *)progname;
	else
		__progname = (char *)tmpn + 1;
}
