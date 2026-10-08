/*
 * Copyright (c) 2026 Daniel Boland <dboland@xs4all.nl>.
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

/* wincon.h */

#define KeyEvent			Event.KeyEvent
#define MouseEvent			Event.MouseEvent
#define WindowBufferSizeEvent		Event.WindowBufferSizeEvent
#define MenuEvent			Event.MenuEvent
#define FocusEvent			Event.FocusEvent

#define ENABLE_VIRTUAL_TERMINAL_INPUT		0x0200

#define ENABLE_VIRTUAL_TERMINAL_PROCESSING	0x0004
#define ENABLE_LVB_GRID_WORLDWIDE		0x0010
#define DISABLE_NEWLINE_AUTO_RETURN		0x0008

#define COMMON_LVB_LEADING_BYTE		0x0100
#define COMMON_LVB_TRAILING_BYTE	0x0200
#define COMMON_LVB_GRID_HORIZONTAL	0x0400
#define COMMON_LVB_GRID_LVERTICAL	0x0800
#define COMMON_LVB_GRID_RVERTICAL	0x1000
#define COMMON_LVB_AUTOWRAP		0x2000
#define COMMON_LVB_REVERSE_VIDEO	0x4000
#define COMMON_LVB_UNDERSCORE		0x8000

/* sys/termios.h */

#define WIN_ECHO	0x00000008
#define WIN_ISIG	0x00000080
#define WIN_ICANON	0x00000100

/* line In */

#define WIN_IGNBRK	0x00000001      /* ignore BREAK condition */
#define WIN_BRKINT	0x00000002      /* map BREAK to SIGINT */
#define WIN_IXON	0x00000200	/* enable output flow control */
#define WIN_IXOFF	0x00000400	/* enable input flow control */
#define WIN_INLCR	0x00000040	/* Ye Olde TTY had separate key for CR */
#define WIN_ICRNL	0x00000100	/* map CR to NL (ala CRMOD) */
#define WIN_IEXTEN	0x00000400      /* enable DISCARD and LNEXT */

/* line Out */

#define WIN_OPOST	0x00000001	/* enable following output processing */
#define WIN_ONLCR	0x00000002	/* map NL to CR-NL (ala CRMOD) */
#define WIN_OXTABS	0x00000004	/* expand tabs to spaces */
#define WIN_OCRNL	0x00000010	/* map CR to NL */

/* Control */

#define WIN_CS8		0x00000300      /* 8 bits */
#define WIN_CREAD	0x00000800      /* enable receiver */
#define WIN_HUPCL	0x00004000      /* hang up on last close */

#define WIN_POSIX_VDISABLE	(0377)

/* standard speeds */

#define WIN_B9600	9600

/* sys/ttydefaults.h (stty.exe -a) */

#define WIN_TTYDEF_IFLAG	(WIN_BRKINT | WIN_ICRNL)
#define WIN_TTYDEF_OFLAG	(WIN_OPOST | WIN_ONLCR | WIN_OXTABS)
#define WIN_TTYDEF_LFLAG	(WIN_ECHO | WIN_ICANON | WIN_ISIG | WIN_IEXTEN)
#define WIN_TTYDEF_CFLAG	(WIN_CREAD | WIN_CS8 | WIN_HUPCL)
#define WIN_TTYDEF_SPEED	(WIN_B9600)

#define CTRL(x) (x&037)
#define WIN_CEOF            CTRL('d')
#define WIN_CEOL            ((unsigned char)'\377') /* XXX avoid _POSIX_VDISABLE */
#define WIN_CERASE          010
#define WIN_CINTR           CTRL('c')
#define WIN_CSTATUS         ((unsigned char)'\377') /* XXX avoid _POSIX_VDISABLE */
#define WIN_CKILL           CTRL('u')
#define WIN_CMIN            1
#define WIN_CQUIT           034             /* FS, ^\ */
#define WIN_CSUSP           CTRL('z')
#define WIN_CTIME           0
#define WIN_CDSUSP          CTRL('y')
#define WIN_CSTART          CTRL('q')
#define WIN_CSTOP           CTRL('s')
#define WIN_CLNEXT          CTRL('v')
#define WIN_CDISCARD        CTRL('o')
#define WIN_CWERASE         CTRL('w')
#define WIN_CREPRINT        CTRL('r')
#define WIN_CEOT            CEOF

/* sys/ttycom.h */

#define TIOCFLAG_ACTIVE		0x00010000

