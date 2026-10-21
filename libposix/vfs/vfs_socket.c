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
vfs_socket(INT Family, INT Type, INT Protocol, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;

	switch (Family){
		case WS_AF_LOCAL:
			bResult = pipe_socket(Family, Type, Protocol, Result);
			break;
		case WS_AF_ROUTE:
			bResult = route_socket(Family, Type, Protocol, Result);
			break;
		default:
			bResult = ws2_socket(Family, Type, Protocol, Result);
	}
	return(bResult);
}
BOOL 
vfs_connect(WIN_VNODE *Node, CONST LPSOCKADDR Address, INT Size)
{
	BOOL bResult = FALSE;

	switch (Node->FSType){
		case FS_TYPE_PIPE:
			bResult = pipe_connect(Node, Address, Size);
			break;
		case FS_TYPE_WINSOCK:
			bResult = ws2_connect(Node, Address, Size);
			break;
		default:
			WSASetLastError(WSAEPFNOSUPPORT);
	}
	return(bResult);
}
BOOL 
vfs_bind(WIN_TASK *Task, WIN_VNODE *Node, LPSOCKADDR Address, INT Length)
{
	BOOL bResult = FALSE;

	switch (Node->FSType){
		case FS_TYPE_PIPE:
			bResult = pipe_bind(Task, Node, Address, Length);
			break;
		case FS_TYPE_WINSOCK:
			bResult = ws2_bind(Node, Address, Length);
			break;
		default:
			WSASetLastError(WSAEPFNOSUPPORT);
	}
	return(bResult);
}
BOOL 
vfs_listen(WIN_VNODE *Node, INT Backlog)
{
	BOOL bResult = FALSE;

	/* The listen() system call enables the accept() mechanism.
	 */
	switch (Node->FSType){
		case FS_TYPE_WINSOCK:
			bResult = ws2_listen(Node, Backlog);
			break;
		case FS_TYPE_PIPE:
			bResult = pipe_listen(Node, Backlog);
			break;
		default:
			WSASetLastError(WSAEPFNOSUPPORT);
	}
	return(bResult);
}
BOOL 
vfs_accept(WIN_TASK *Task, WIN_VNODE *Node, LPSOCKADDR Address, LPINT Length, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;

	Task->State = WIN_SSLEEP;
	switch (Node->FSType){
		case FS_TYPE_WINSOCK:
			bResult = ws2_accept(Task, Node, Address, Length, Result);
			break;
		default:
			WSASetLastError(WSAEPFNOSUPPORT);
	}
	Task->State = WIN_SRUN;
	return(bResult);
}
BOOL 
vfs_sendmsg(WIN_VNODE *Node, WSAMSG *Message, DWORD *Result)
{
	BOOL bResult = FALSE;

	switch (Node->FSType){
		case FS_TYPE_WINSOCK:	/* dig.exe */
			bResult = ws2_sendmsg(Node, Message, Result);
			break;
		case FS_TYPE_PIPE:
			bResult = pipe_sendmsg(Node, Message, Result);
			break;
		default:
			WSASetLastError(WSAEPFNOSUPPORT);
	}
	return(bResult);
}
BOOL 
vfs_recvmsg(WIN_VNODE *Node, WSAMSG *Message, DWORD *Result)
{
	BOOL bResult = FALSE;

	switch (Node->FSType){
		case FS_TYPE_WINSOCK:
			bResult = ws2_recvmsg(Node, Message, Result);
			break;
		case FS_TYPE_PIPE:
			bResult = pipe_recvmsg(Node, Message, Result);
			break;
		default:
			WSASetLastError(WSAEPFNOSUPPORT);
	}
	return(bResult);
}
BOOL 
vfs_socketpair(INT Family, DWORD Attribs, INT Protocol, WIN_VNODE Result[2])
{
	BOOL bResult = FALSE;

	if (Family == WS_AF_LOCAL){
		bResult = pipe_socketpair(Family, Attribs, Protocol, Result);
	}else{
		WSASetLastError(WSAEPFNOSUPPORT);
	}
	return(bResult);
}

/****************************************************/

BOOL 
vfs_recvfrom(WIN_VNODE *Node, LPSTR Buffer, UINT Size, DWORD Flags, LPSOCKADDR From, UINT *FromLen, DWORD *Result)
{
	BOOL bResult = FALSE;

	switch (Node->FSType){
		case FS_TYPE_WINSOCK:
			bResult = ws2_recvfrom(Node, Buffer, Size, Flags, From, FromLen, Result);
			break;
		case FS_TYPE_PIPE:
			bResult = pipe_recvfrom(Node, Buffer, Size, Flags, Result);
			break;
		default:
			WSASetLastError(WSAEPFNOSUPPORT);
	}
	return(bResult);
}
BOOL 
vfs_sendto(WIN_VNODE *Node, LPCSTR Buffer, UINT Size, DWORD Flags, CONST LPSOCKADDR To, UINT ToLen, DWORD *Result)
{
	BOOL bResult = FALSE;

	switch (Node->FSType){
		case FS_TYPE_WINSOCK:
			bResult = ws2_sendto(Node, Buffer, Size, Flags, To, ToLen, Result);
			break;
		case FS_TYPE_PIPE:
			bResult = pipe_sendto(Node, Buffer, Size, Flags, Result);
			break;
		default:
			WSASetLastError(WSAEPFNOSUPPORT);
	}
	return(bResult);
}
BOOL 
vfs_shutdown(WIN_VNODE *Node, INT How)
{
	BOOL bResult = FALSE;

	switch (Node->FSType){
		case FS_TYPE_WINSOCK:
			bResult = ws2_shutdown(Node, How);
			break;
		case FS_TYPE_PDO:	/* route.exe add */
			bResult = TRUE;
			break;
		default:
			WSASetLastError(WSAEPFNOSUPPORT);
	}
	return(bResult);
}
BOOL 
vfs_getsockopt(WIN_VNODE *Node, INT Level, INT Name, CHAR *Value, INT *Length)
{
	BOOL bResult = FALSE;

	switch (Node->FSType){
		case FS_TYPE_WINSOCK:
			bResult = ws2_getsockopt(Node, Level, Name, Value, Length);
			break;
		case FS_TYPE_PIPE:
			bResult = pipe_getsockopt(Node, Level, Name, Value, Length);
			break;
		default:
			WSASetLastError(WSAEPFNOSUPPORT);
	}
	return(bResult);
}
BOOL 
vfs_setsockopt(WIN_VNODE *Node, INT Level, INT Name, CONST CHAR *Value, INT Length)
{
	BOOL bResult = FALSE;

	switch (Node->FSType){
		case FS_TYPE_WINSOCK:
			bResult = ws2_setsockopt(Node, Level, Name, Value, Length);
			break;
		case FS_TYPE_PIPE:
			bResult = pipe_setsockopt(Node, Level, Name, Value, Length);
			break;
		default:
			WSASetLastError(WSAEPFNOSUPPORT);
	}
	return(bResult);
}
BOOL 
vfs_getpeername(WIN_VNODE *Node, LPSOCKADDR Name, INT *Length)
{
	BOOL bResult = FALSE;

	switch (Node->FSType){
		case FS_TYPE_WINSOCK:
			bResult = ws2_getpeername(Node, Name, Length);
			break;
		default:
			WSASetLastError(WSAEPFNOSUPPORT);
	}
	return(bResult);
}
BOOL 
vfs_getsockname(WIN_VNODE *Node, LPSOCKADDR Name, INT *Length)
{
	BOOL bResult = FALSE;

	switch (Node->FSType){
		case FS_TYPE_WINSOCK:
			bResult = ws2_getsockname(Node, Name, Length);
			break;
		default:
			WSASetLastError(WSAEPFNOSUPPORT);
	}
	return(bResult);
}
