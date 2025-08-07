/*      $OpenBSD: curses.h,v 1.3 2003/06/17 21:33:53 millert Exp $      */

/*
 * Placed in the public domain by Todd C. Miller <Todd.Miller@courtesan.com>
 * on June 17, 2003.
 */

/* Including this file would create a circular reference in ncurses.h.
 * Luckily, we have an #include guard in place. Taken from OpenBSD 4.1.
 */
#ifndef _CURSES_H_
#define _CURSES_H_

#ifdef _USE_OLD_CURSES_
#include <ocurses.h>
#else
#include <ncurses.h>
#endif

#endif /* _CURSES_H_ */
