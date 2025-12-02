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
char_read(WIN_TASK *Task, WIN_VNODE *Node, LPSTR Buffer, LONG Size, DWORD *Result)
{
	BOOL bResult = FALSE;

	switch (Node->DeviceType){
		case DEV_TYPE_PTY:
		case DEV_TYPE_CONSOLE:
			bResult = input_read(Task, TERMINAL(Node->Index), Buffer, Size, Result);
			break;
		case DEV_TYPE_INPUT:
		case DEV_TYPE_NULL:
			bResult = ReadFile(Node->Handle, Buffer, Size, Result, NULL);
			break;
		default:
			SetLastError(ERROR_BAD_DEVICE);
	}
	return(bResult);
}
BOOL 
char_write(WIN_VNODE *Node, LPCSTR Buffer, DWORD Size, DWORD *Result)
{
	BOOL bResult = FALSE;

	switch (Node->DeviceType){
		case DEV_TYPE_PTY:
		case DEV_TYPE_CONSOLE:
			bResult = screen_write(TERMINAL(Node->Index), Buffer, Size, Result);
			break;
		case DEV_TYPE_SCREEN:
		case DEV_TYPE_NULL:
			bResult = WriteFile(Node->Handle, Buffer, Size, Result, NULL);
			break;
		default:
			SetLastError(ERROR_BAD_DEVICE);
	}
	return(bResult);
}
BOOL 
char_fsync(WIN_VNODE *Node)
{
	BOOL bResult = FALSE;

	switch (Node->DeviceType){
		case DEV_TYPE_PTY:
		case DEV_TYPE_CONSOLE:		/* less.exe */
			bResult = con_fsync(TERMINAL(Node->Index));
			break;
		case DEV_TYPE_INPUT:
			bResult = FlushConsoleInputBuffer(Node->Handle);
			break;
		default:
			SetLastError(ERROR_BAD_DEVICE);
	}
	return(bResult);
}
BOOL 
char_revoke(WIN_VNODE *Node)
{
	BOOL bResult = FALSE;

	switch (Node->DeviceType){
		case DEV_TYPE_CONSOLE:
			bResult = TRUE;
			break;
		case DEV_TYPE_PTY:
			bResult = con_revoke(TERMINAL(Node->Index), DEVICE(Node->DeviceId));
			break;
		default:
			SetLastError(ERROR_BAD_DEVICE);
	}
	return(bResult);
}
