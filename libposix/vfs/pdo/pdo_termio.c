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
pdo_TIOCFLUSH(WIN_DEVICE *Device)
{
	BOOL bResult = FALSE;

	switch (Device->DeviceType){
//		case DEV_TYPE_PTY:
//		case DEV_TYPE_CONSOLE:
//			bResult = input_TIOCFLUSH(Device->Input);
//			break;
		case DEV_TYPE_TTY:
			bResult = TRUE;
			break;
		default:
			SetLastError(ERROR_BAD_DEVICE);
	}
	return(bResult);
}
BOOL 
pdo_TIOCDRAIN(WIN_DEVICE *Device)
{
	BOOL bResult = FALSE;

	switch (Device->DeviceType){
//		case DEV_TYPE_PTY:
//		case DEV_TYPE_CONSOLE:
//			bResult = screen_TIOCDRAIN(Device->Output);
//			break;
		case DEV_TYPE_TTY:
			bResult = TRUE;
			break;
		default:
			SetLastError(ERROR_BAD_DEVICE);
	}
	return(bResult);
}
BOOL 
pdo_TIOCSETA(WIN_DEVICE *Device, WIN_TERMIO *Attribs)
{
	BOOL bResult = FALSE;

	switch (Device->DeviceType){
//		case DEV_TYPE_PTY:
//		case DEV_TYPE_CONSOLE:
//			bResult = con_TIOCSETA(Device, Attribs);
//			break;
		case DEV_TYPE_TTY:
			bResult = TRUE;
			break;
		default:
			SetLastError(ERROR_BAD_DEVICE);
	}
	return(bResult);
}
BOOL 
pdo_PTMGET(WIN_DEVICE *Master, WIN_DEVICE *Slave)
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
		Slave->Index = Master->Index;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
pdo_TIOCSCTTY(WIN_DEVICE *Device, WIN_TTY **Result)
{
	BOOL bResult = FALSE;

	switch (Device->DeviceType){
		case DEV_TYPE_PTY:
		case DEV_TYPE_CONSOLE:
			bResult = con_TIOCSCTTY(Device, Result);
			break;
//		case DEV_TYPE_PTY:
//			bResult = mail_TIOCSCTTY(Terminal);
//			break;
		default:
			SetLastError(ERROR_BAD_DEVICE);
	}
	return(bResult);
}
