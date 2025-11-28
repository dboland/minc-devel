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
mail_PTMGET(WIN_DEVICE *Master, WIN_DEVICE *Slave)
{
	BOOL bResult = FALSE;
	HANDLE hMaster = Master->Output;
	HANDLE hSlave = Slave->Output;

	if (Slave->Flags & WIN_DVF_ACTIVE){
		SetLastError(ERROR_NOT_READY);
	}else{
		Master->Output = hSlave;
		Slave->Output = hMaster;
		Slave->Flags |= WIN_DVF_ACTIVE;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
mail_TIOCSCTTY(WIN_DEVICE *Device, WIN_TTY *Terminal)
{
	BOOL bResult = FALSE;
	HANDLE hResult = NULL;
	SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};
	CHAR szPath[MAX_PATH] = "\\\\.\\MAILSLOT\\slave\\";

	hResult = CreateMailslot(win_strcat(szPath, Device->Name), WIN_MAX_INPUT, MAILSLOT_WAIT_FOREVER, &sa);
	if (hResult == INVALID_HANDLE_VALUE){
		WIN_ERR("CreateMailslot(%s): %s\n", szPath, win_strerror(GetLastError()));
	}else{
		Device->Input = hResult;
		Device->Output = MailOpenFile(szPath);
		bResult = TRUE;
	}
	return(bResult);
}
