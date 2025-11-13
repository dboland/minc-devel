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

#include "../config.h"

LPSTR 
VfsScreenMode(LPSTR Buffer, LPCSTR Label, DWORD Mode)
{
	LPSTR psz = Buffer;
	DWORD dwRemain = Mode;

	psz += sprintf(psz, "%s(0x%x): ", Label, dwRemain);
	psz = WinFlagName(psz, ENABLE_PROCESSED_OUTPUT, "PROCESSED_OUTPUT", dwRemain, &dwRemain);
	psz = WinFlagName(psz, ENABLE_WRAP_AT_EOL_OUTPUT, "WRAP_AT_EOL_OUTPUT", dwRemain, &dwRemain);
	psz = WinFlagName(psz, ENABLE_VIRTUAL_TERMINAL_PROCESSING, "VIRTUAL_TERMINAL_PROCESSING", dwRemain, &dwRemain);
	psz = WinFlagName(psz, DISABLE_NEWLINE_AUTO_RETURN, "DISABLE_NEWLINE_AUTO_RETURN", dwRemain, &dwRemain);
	psz = WinFlagName(psz, ENABLE_LVB_GRID_WORLDWIDE, "LVB_GRID_WORLDWIDE", dwRemain, &dwRemain);
	psz += sprintf(psz, " [0x%x]\n", dwRemain);
	return(psz);
}
