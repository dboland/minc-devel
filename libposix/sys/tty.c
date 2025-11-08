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

#include <sys/tty.h>

#undef TTYDEF_IFLAG
#define TTYDEF_IFLAG    (ICRNL | IMAXBEL | IXON | IXANY)

/****************************************************/

void 
tty_init(void)
{
	WIN_TTY *pwTerminal = __Terminals;
	DWORD dwIndex = 0;
	struct termios attr = {TTYDEF_IFLAG, TTYDEF_OFLAG, TTYDEF_CFLAG, 
		TTYDEF_LFLAG, {0}, TTYDEF_SPEED, TTYDEF_SPEED};

	win_memcpy(attr.c_cc, ttydefchars, sizeof(ttydefchars));
	while (dwIndex < WIN_TTY_MAX){
		win_memcpy(&pwTerminal->Attribs, &attr, sizeof(WIN_TERMIO));
		pwTerminal->ScrollRate = 1;
		pwTerminal++;
		dwIndex++;
	}
	tty_attach(DEVICE(DEV_TYPE_CONSOLE));
}
