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
pipe_socket(INT Family, INT Type, INT Protocol, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;

	Result->Handle = INVALID_HANDLE_VALUE;
	Result->FSType = FS_TYPE_PIPE;
	Result->FileType = WIN_VSOCK;
	Result->DeviceType = DEV_CLASS_CPU;
	Result->Access = GENERIC_READ + GENERIC_WRITE;
	return(TRUE);
}
BOOL 
pipe_bind(WIN_TASK *Task, WIN_VNODE *Node, LPSOCKADDR Name, INT Length)
{
	BOOL bResult = FALSE;
	WIN_MODE wMode = {WIN_VSOCK, WIN_S_IRW, WIN_S_IRW, WIN_S_IRW, 0};
	WCHAR szName[MAX_NAME];
	DWORD dwAttribs = FILE_FLAG_OVERLAPPED | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE;

	if (!PipeCreateFile(VfsCreateName(szName), dwAttribs, __PipeEvent, Node)){
		return(FALSE);
	}else if (pipe_mknod(Task, (LPWSTR)Name->sa_data, &wMode, szName)){
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
pipe_listen(WIN_VNODE *Node, INT Backlog)
{
	/* saslauthd.exe
	 */
	return(TRUE);
}
BOOL 
pipe_connect(WIN_VNODE *Node, CONST LPSOCKADDR Name, INT Length)
{
	BOOL bResult = FALSE;

	if (PipeOpenFile((LPWSTR)Name->sa_data, __PipeEvent, Node)){
		bResult = TRUE;
//	}else{
//		WIN_ERR("pipe_connect(%ls): %s\n", Name->sa_data, win_strerror(GetLastError()));
	}
	return(bResult);
}
BOOL 
pipe_socketpair(INT Domain, INT Mode, INT Protocol, WIN_VNODE Result[2])
{
	BOOL bResult = FALSE;
	WCHAR szName[MAX_GUID];
	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (!PipeCreateFile(VfsCreateName(szName), Mode, hEvent, &Result[0])){
		return(FALSE);
	}else if (PipeOpenFile(szName, hEvent, &Result[1])){
		Result[0].FileType = WIN_VSOCK;
		Result[0].DeviceType = DEV_CLASS_CPU;
		Result[1].FileType = WIN_VSOCK;
		Result[1].DeviceType = DEV_CLASS_CPU;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
pipe_sendto(WIN_VNODE *Node, LPCSTR Buffer, UINT Size, DWORD Flags, DWORD *Result)
{
	return(fifo_write(Node, Buffer, Size, Result));
}
BOOL 
pipe_recvfrom(WIN_VNODE *Node, LPSTR Buffer, UINT Size, DWORD Flags, DWORD *Result)
{
	return(fifo_read(Node, Buffer, Size, Result));
}
BOOL 
pipe_sendmsg(WIN_VNODE *Node, WSAMSG *Message, DWORD *Result)
{
	BOOL bResult = FALSE;
	DWORD dwSize, dwResult;
	PVOID pvData, P;

	pvData = PipeAllocData(Message, &dwSize, &dwResult);
	P = PipePushDatagram(pvData, Message->lpBuffers, Message->dwBufferCount);
	P = PipePushDatagram(P, &Message->Control, 1);
	if (!fifo_write(Node, pvData, dwSize, &dwSize)){
		WIN_ERR("pipe_sendmsg(%d): %s\n", Node->Handle, win_strerror(GetLastError()));
	}else{
		*Result = dwResult;
		bResult = TRUE;
	}
	win_free(pvData);
	return(bResult);
}
BOOL 
pipe_recvmsg(WIN_VNODE *Node, WSAMSG *Message, DWORD *Result)
{
	BOOL bResult = FALSE;
	DWORD dwSize, dwResult;
	PVOID pvData;

	pvData = PipeAllocData(Message, &dwSize, &dwResult);
	if (!fifo_read(Node, pvData, dwSize, &dwSize)){
		WIN_ERR("pipe_recvmsg(%d): %s\n", Node->Handle, win_strerror(GetLastError()));
	}else{
		pvData = PipePopDatagram(pvData, Message->lpBuffers, Message->dwBufferCount);
		PipePopDatagram(pvData, &Message->Control, 1);
		*Result = dwResult;
		bResult = TRUE;
	}
	win_free(pvData);
	return(bResult);
}
BOOL 
pipe_getsockopt(WIN_VNODE *Node, INT Level, INT Name, CHAR *Value, INT *Length)
{
	BOOL bResult = TRUE;

	switch (Name){
		case SO_RCVBUF:
		case SO_SNDBUF:
			*(UINT *)Value = WIN_PIPE_BUF;
			*Length = sizeof(UINT);
			break;
		default:
			WSASetLastError(WSAENOPROTOOPT);
			bResult = FALSE;
	}
	return(bResult);
}
BOOL 
pipe_setsockopt(WIN_VNODE *Node, INT Level, INT Name, CONST CHAR *Value, INT Length)
{
	BOOL bResult = TRUE;

	switch (Name){
		case SO_RCVBUF:
		case SO_SNDBUF:
			break;
		default:
			WSASetLastError(WSAENOPROTOOPT);
			bResult = FALSE;
	}
	return(bResult);
}
