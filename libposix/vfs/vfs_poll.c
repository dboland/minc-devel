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

/************************************************************/

DWORD 
PollGetObjects(WIN_VNODE *Nodes[], HANDLE Result[])
{
	DWORD dwResult = 0;
	WIN_VNODE *pNode;

	Result[dwResult++] = __Interrupt;
	while (pNode = *Nodes++){
		Result[dwResult++] = pNode->Event;
	}
	return(dwResult);
}
BOOL 
PollNode(WIN_VNODE *Node, WIN_POLLFD *Info, DWORD *Result)
{
	BOOL bResult = FALSE;

	switch (Node->FSType){
		case FS_TYPE_CHAR:
			bResult = char_poll(Node, Info, Result);
			break;
		case FS_TYPE_PIPE:
			bResult = pipe_poll(Node, Info, Result);
			break;
		case FS_TYPE_PDO:
			bResult = pdo_poll(DEVICE(Node->DeviceId), Info, Result);
			break;
		case FS_TYPE_MAILSLOT:
			bResult = mail_poll(TERMINAL(Node->Index), Info, Result);
			break;
		case FS_TYPE_DISK:
			bResult = disk_poll(Node->Handle, Info, Result);
			break;
		case FS_TYPE_WINSOCK:
			bResult = ws2_poll(Node, Info, Result);
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
BOOL 
PollNoWait(WIN_VNODE *Nodes[], WIN_POLLFD *Info[], DWORD *Result)
{
	WIN_VNODE *pvNode;
	DWORD dwResult = 0;

	while (pvNode = *Nodes++){
		if (!PollNode(pvNode, *Info++, &dwResult)){
			return(FALSE);
		}
	}
	*Result = dwResult;
	return(TRUE);
}
BOOL 
PollWait(WIN_TASK *Task, WIN_VNODE *Nodes[], DWORD *TimeOut)
{
	BOOL bResult = FALSE;
	DWORD dwStatus = 0;
	HANDLE hObjects[WSA_MAXIMUM_WAIT_EVENTS];
	DWORD dwCount = PollGetObjects(Nodes, hObjects);
	LONGLONG llRemain = (LONGLONG)GetTickCount();

	if (*TimeOut != INFINITE){
		llRemain += *TimeOut;
	}
	dwStatus = WSAWaitForMultipleEvents(dwCount, hObjects, FALSE, *TimeOut, TRUE);
	if (dwStatus == WSA_WAIT_FAILED){
		WIN_ERR("WSAWaitForMultipleEvents(%s): %s\n", win_strobj(hObjects, dwCount), win_strerror(WSAGetLastError()));
	}else{
		bResult = TRUE;
	}
	llRemain -= (LONGLONG)GetTickCount();
	if (llRemain > 0){
		*TimeOut = llRemain;
	}else if (*TimeOut != INFINITE){
		*TimeOut = 0;
	}
	return(bResult);
}

/************************************************************/

BOOL 
vfs_poll(WIN_TASK *Task, WIN_VNODE *Nodes[], WIN_POLLFD *Info[], DWORD *TimeOut, DWORD *Result)
{
	BOOL bResult = FALSE;

	Task->State = WIN_SSLEEP;
	while (!bResult){
		if (!PollNoWait(Nodes, Info, Result)){
			break;
		}else if (*Result){
			bResult = TRUE;
		}else if (!*TimeOut){
			bResult = TRUE;
		}else if (!PollWait(Task, Nodes, TimeOut)){
			break;
		}else if (proc_poll(Task)){
			break;
		}
	}
	Task->State = WIN_SONPROC;
	return(bResult);
}
