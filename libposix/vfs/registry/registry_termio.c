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

#include <winreg.h>

/****************************************************/

BOOL 
reg_TIOCGETA(LPCSTR Path, DWORD Mode[2])
{
	BOOL bResult = FALSE;
	DWORD dwType = REG_DWORD;
	DWORD dwValue = 0;
	DWORD dwSize = sizeof(DWORD);
	HKEY hKey;

	if (ERROR_SUCCESS != RegOpenKey(HKEY_CURRENT_USER, Path, &hKey)){
		WIN_ERR("RegOpenKey(%s): %s\n", Path, win_strerror(GetLastError()));
	}else if (ERROR_SUCCESS == RegQueryValueEx(hKey, "VirtualTerminalLevel", NULL, &dwType, (PBYTE)&dwValue, &dwSize)){
		if (dwValue > 0){
			Mode[0] |= ENABLE_VIRTUAL_TERMINAL_INPUT;
			Mode[1] |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		}
		RegCloseKey(hKey);
		bResult = TRUE;
	}
	return(bResult);
}
