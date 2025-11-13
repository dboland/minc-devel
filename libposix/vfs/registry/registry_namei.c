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

#define DRIVER_ROOT	L"SYSTEM\\CurrentControlSet\\Enum\\"
#define CLASS_ROOT	L"SYSTEM\\CurrentControlSet\\Control\\Class\\"
#define INTERFACE_ROOT	L"SYSTEM\\CurrentControlSet\\Control\\DeviceClasses\\"
#define MOUNT_ROOT	L"SYSTEM\\MountedDevices\\"
#define CONSOLE_ROOT	L"Console\\"
#define BIOS_ROOT	L"HARDWARE\DESCRIPTION\System\\"

/****************************************************/

LPWSTR 
RegReadPath(LPCWSTR Source, LPWSTR Destination)
{
	WCHAR C;
	LONG lDepth = 0;
	WCHAR *pszBase = NULL;

	while (C = *Source++){
		if (C == '#'){
			if (lDepth < 2){
				C = '\\';
			}
			lDepth++;
			pszBase = Destination;
		}
		*Destination++ = C;
	}
	if (pszBase){
		*pszBase++ = 0;
	}
	*Destination = 0;
	return(pszBase);
}
/****************************************************/

WIN_NAMEIDATA *
reg_lookup(WIN_NAMEIDATA *Result, DWORD FileType, LPCWSTR Source)
{
	Result->FSType = FS_TYPE_REGISTRY;
	switch (FileType){
		case REG_DRIVER:
			Result->R = win_wcpcpy(Result->Resolved, DRIVER_ROOT);
			break;
		case REG_CLASS:
			Result->R = win_wcpcpy(Result->Resolved, CLASS_ROOT);
			break;
		case REG_IFACE:
			Result->R = win_wcpcpy(Result->Resolved, INTERFACE_ROOT);
			break;
		case REG_MOUNT:
			Result->R = win_wcpcpy(Result->Resolved, MOUNT_ROOT);
			break;
		case REG_TTY:
			Result->R = win_wcpcpy(Result->Resolved, CONSOLE_ROOT);
			break;
		default:
			Result->R = Result->Resolved;
	}
	Result->FileType = FileType;
	Result->R = RegReadPath(Source, Result->R);
	return(Result);
}
