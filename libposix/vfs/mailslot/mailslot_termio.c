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

#include <winbase.h>

/****************************************************/

BOOL 
mail_PTMGET(WIN_DEVICE *Device, WIN_TTY *Terminal, WIN_PTMGET *Result)
{
	BOOL bResult = FALSE;
	HANDLE hResult = NULL;
	CHAR szPath[MAX_PATH] = "\\\\.\\MAILSLOT\\master\\";
	SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};

	hResult = CreateMailslot(win_strcat(szPath, Device->Name), WIN_MAX_INPUT, MAILSLOT_WAIT_FOREVER, &sa);
	if (hResult == INVALID_HANDLE_VALUE){
		WIN_ERR("CreateMailslot(%s): %s\n", szPath, win_strerror(GetLastError()));
	}else{
		Device->Input = hResult;
		Device->Output = MailOpenFile(szPath);
		Device->FSType = FS_TYPE_MAILSLOT;
		Device->Event = CreateEvent(&sa, FALSE, FALSE, NULL);
		win_strlcpy(Result->SName, Device->Name, 16);
		win_strlcpy(Result->MName, Terminal->Name, 16);
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
mail_TIOCSCTTY(WIN_DEVICE *Device, WIN_TASK *Task, WIN_TTY *Terminal)
{
	BOOL bResult = FALSE;
	HANDLE hResult = NULL;
	SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};
	CHAR szPath[MAX_PATH] = "\\\\.\\MAILSLOT\\slave\\";

	hResult = CreateMailslot(win_strcat(szPath, Device->Name), WIN_MAX_INPUT, MAILSLOT_WAIT_FOREVER, &sa);
	if (hResult == INVALID_HANDLE_VALUE){
		WIN_ERR("CreateMailslot(%s): %s\n", szPath, win_strerror(GetLastError()));
	}else{
		Terminal->Input = hResult;
		Terminal->Output = Device->Output;
		Terminal->Event = Device->Event;
		Terminal->FSType = FS_TYPE_MAILSLOT;
		Device->Output = MailOpenFile(szPath);
		Terminal->SessionId = Task->SessionId;
		Terminal->GroupId = Task->GroupId;
		Task->Flags |= WIN_PS_CONTROLT;
		Task->CTTY = Terminal->Index;
		bResult = TRUE;
	}
	return(bResult);
}
