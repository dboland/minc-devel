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

#include <ws2tcpip.h>

#define WS_SO_TIMESTAMP		0x800

GUID WSAID_WSASENDMSG = {0xa441e712,0x754f,0x43ca,{0x84,0xa7,0x0d,0xee,0x44,0xcf,0x60,0x6d}};
GUID WSAID_WSARECVMSG = {0xf689d7c8,0x6f1f,0x436b,{0x8a,0x53,0xe5,0x4f,0xe3,0x51,0xc3,0x22}};

typedef INT (WINSOCK_API_LINKAGE *LPFN_WSASENDMSG)(SOCKET,LPWSAMSG,DWORD,LPDWORD,LPWSAOVERLAPPED,LPWSAOVERLAPPED_COMPLETION_ROUTINE);
typedef INT (WINSOCK_API_LINKAGE *LPFN_WSARECVMSG)(SOCKET,LPWSAMSG,LPDWORD,LPWSAOVERLAPPED,LPWSAOVERLAPPED_COMPLETION_ROUTINE);

/****************************************************/

BOOL 
ws2_setsockopt_SOL(WIN_VNODE *Node, INT Name, CONST CHAR *Value)
{
	BOOL bResult = FALSE;

	/* sys/socket.h
	 */
	Node->Options |= Name;
	switch (Name){
		case WS_SO_TIMESTAMP:	/* dig.exe */
			bResult = TRUE;
			break;
		default:
			WIN_ERR("setsockopt(SOL_SOCKET): Name(0x%x): %s\n", Name, win_strerror(WSAGetLastError()));
	}
	return(bResult);
}

/****************************************************/

BOOL 
ws2_socket(UINT Family, UINT Type, INT Protocol, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;
	SOCKET sResult;

	/* WSASocket() needs the "SystemRoot" environment variable
	 * to be present (WSAEPROVIDERFAILEDINIT).
	 */
	sResult = WSASocket(Family, Type, Protocol, NULL, 0, 0);
	if (sResult != INVALID_SOCKET){
		Result->Socket = sResult;
		Result->FSType = FS_TYPE_WINSOCK;
		Result->FileType = WIN_VSOCK;
		Result->Event = WSACreateEvent();
		Result->Pending = 0;
		Result->DeviceType = DEV_TYPE_ETH;
		Result->Access = win_F_GETFL((HANDLE)sResult);
		Result->Flags = win_F_GETFD((HANDLE)sResult);
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
ws2_connect(WIN_VNODE *Node, CONST LPSOCKADDR Address, INT Length)
{
	BOOL bResult = FALSE;

	/* see: http://cr.yp.to/docs/connect.html (D.J. Bernstein, 2000)
	 * Git (libcurl) and Perl use the SO_ERROR/getpeername() approach.
	 * Lynx selects() for write, using a tiny timeout (100 ms),
	 * gets it, but tries to connect() anyway.
	 */
	if (SOCKET_ERROR != connect(Node->Socket, Address, Length)){
		bResult = TRUE;
	}else if (WSAEWOULDBLOCK == WSAGetLastError()){
		WSASetLastError(WSAEINPROGRESS);
	}else if (WSAEINVAL == WSAGetLastError()){	/* lynx.exe */
		WSASetLastError(WSAEISCONN);
	}
	return(bResult);
}
BOOL 
ws2_bind(WIN_VNODE *Node, LPSOCKADDR Address, INT Length)
{
	BOOL bResult = FALSE;

	if (SOCKET_ERROR != bind(Node->Socket, Address, Length)){
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
ws2_listen(WIN_VNODE *Node, INT Backlog)
{
	BOOL bResult = FALSE;

	if (SOCKET_ERROR != listen(Node->Socket, Backlog)){
		bResult = TRUE;
	}else{
		WIN_ERR("listen(%d): %s\n", Node->Socket, win_strerror(WSAGetLastError()));
	}
	return(bResult);
}
BOOL 
ws2_accept(WIN_TASK *Task, WIN_VNODE *Node, LPSOCKADDR Address, LPINT Length, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;
	WIN_POLLFD fdInfo = {Node->FileId, WIN_POLLRDBAND | WIN_POLLIN, 0};
	DWORD dwResult = 0;

	while (!bResult){
		if (!ws2_poll(Node, &fdInfo, &dwResult)){
			break;
		}else if (dwResult){
			bResult = WSAcceptFile(Node, Address, Length, Result);
		}else if (!sock_select(Task, Node, INFINITE)){
			break;
		}
	}
	return(bResult);
}
BOOL 
ws2_sendmsg(WIN_VNODE *Node, WSAMSG *Msg, LPDWORD Result)
{
	BOOL bResult = FALSE;
	LPFN_WSASENDMSG WSASendMsg;
	DWORD dwResult;

	if (SOCKET_ERROR == WSAIoctl(Node->Socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &WSAID_WSASENDMSG, sizeof(GUID), &WSASendMsg, sizeof(WSASendMsg), &dwResult, NULL, NULL)){
		bResult = WSASendMsgProc(Node, Msg, Result, NULL, NULL);
	}else if (SOCKET_ERROR != WSASendMsg(Node->Socket, Msg, Msg->dwFlags, Result, NULL, NULL)){
		bResult = TRUE;
//	}else{
//		WIN_ERR("WSASendMsg(%d): %s\n", Node->Socket, win_strerror(WSAGetLastError()));
	}
	return(bResult);
}
BOOL 
ws2_recvmsg(WIN_VNODE *Node, WSAMSG *Msg, LPDWORD Result)
{
	BOOL bResult = FALSE;
	LPFN_WSARECVMSG WSARecvMsg;
	DWORD dwResult;

	if (SOCKET_ERROR == WSAIoctl(Node->Socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &WSAID_WSARECVMSG, sizeof(GUID), &WSARecvMsg, sizeof(WSARecvMsg), &dwResult, NULL, NULL)){
		bResult = WSARecvMsgProc(Node, Msg, Result, NULL, NULL);
	}else if (SOCKET_ERROR != WSARecvMsg(Node->Socket, Msg, Result, NULL, NULL)){
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
ws2_sendto(WIN_VNODE *Node, LPCSTR Buffer, UINT Size, DWORD Flags, CONST LPSOCKADDR To, UINT ToLen, DWORD *Result)
{
	BOOL bResult = TRUE;
	UINT uiResult = 0;
	UINT uiFlags = Flags & ~WS_MSG_NOSIGNAL;	/* git.exe */

	/* On WinXP SP2 we get "A blocking operation was interrupted by
	 * a call to WSACancelBlockingCall" (traceroute.exe).
	 */
	uiResult = sendto(Node->Socket, Buffer, Size, uiFlags, To, ToLen);
	if (uiResult != SOCKET_ERROR){
		*Result = uiResult;
	}else{
		bResult = FALSE;
	}
	return(bResult);
}
BOOL 
ws2_recvfrom(WIN_VNODE *Node, LPSTR Buffer, UINT Size, DWORD Flags, LPSOCKADDR From, UINT *FromLen, DWORD *Result)
{
	BOOL bResult = TRUE;
	UINT uiResult;

	uiResult = recvfrom(Node->Socket, Buffer, Size, Flags, From, FromLen);
	if (uiResult != SOCKET_ERROR){
		*Result = uiResult;
	}else{
		bResult = FALSE;
	}
	return(bResult);
}
BOOL 
ws2_shutdown(WIN_VNODE *Node, INT How)
{
	BOOL bResult = FALSE;

	if (SOCKET_ERROR != shutdown(Node->Socket, How)){
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
ws2_getsockopt(WIN_VNODE *Node, INT Level, INT Name, CHAR *Value, INT *Length)
{
	BOOL bResult = FALSE;

	if (SOCKET_ERROR != getsockopt(Node->Socket, Level, Name, Value, Length)){
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
ws2_setsockopt(WIN_VNODE *Node, INT Level, INT Name, CONST CHAR *Value, INT Length)
{
	BOOL bResult = FALSE;

	/* netinet/in.h
	 */
	if (SOCKET_ERROR != setsockopt(Node->Socket, Level, Name, Value, Length)){
		bResult = TRUE;
	}else if (Level == SOL_SOCKET){
		bResult = ws2_setsockopt_SOL(Node, Name, Value);
	}else if (Name == -1){		/* BSD-only, must be ignored or implemented */
		bResult = TRUE;
	}else if (Name){		/* Vista-only, can be ignored */
		bResult = TRUE;
//	}else{
//		WIN_ERR("setsockopt(%d): %s\n", Node->Socket, win_strerror(WSAGetLastError()));
	}
	return(bResult);
}
BOOL 
ws2_getpeername(WIN_VNODE *Node, LPSOCKADDR Address, INT *Length)
{
	BOOL bResult = FALSE;

	if (SOCKET_ERROR != getpeername(Node->Socket, Address, Length)){
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
ws2_getsockname(WIN_VNODE *Node, LPSOCKADDR Address, INT *Length)
{
	BOOL bResult = FALSE;

	if (SOCKET_ERROR != getsockname(Node->Socket, Address, Length)){
		bResult = TRUE;
	}
	return(bResult);
}
