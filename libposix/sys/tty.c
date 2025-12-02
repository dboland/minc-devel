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

/****************************************************/

int 
term_USER_NAMEI_TTY(dev_t id, char *buf, size_t buflen)
{
	int result = -ENOENT;
	int index = 0;
	WIN_TTY *pwTerminal = __Terminals;

	while (index < WIN_TTY_MAX){
		if (pwTerminal->DeviceId == id){
			win_strlcpy(buf, pwTerminal->Name, buflen);
			result = 0;
			break;
		}
		index++;
		pwTerminal++;
	}
	return(result);
}

/****************************************************/

VOID 
term_revoke(WIN_TTY *Terminal)
{
	switch (Terminal->DeviceType){
		case DEV_TYPE_CONSOLE:
			break;
		case DEV_TYPE_PTY:
			con_revoke(Terminal, DEVICE(Terminal->DeviceId));
			break;
		default:
			SetLastError(ERROR_CTX_NOT_CONSOLE);
	}
}
