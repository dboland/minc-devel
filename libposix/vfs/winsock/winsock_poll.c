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

#include <mswsock.h>

/****************************************************/

SHORT 
WSAPollEvents(WIN_VNODE *Node, LONG Events)
{
	SHORT sResult = Node->Pending;

	/* In non-blocking mode FD_WRITE needs to be assumed
	 * starting from the first FD_WRITE network event and lasting
	 * until a send returns WSAEWOULDBLOCK.
	 */
	if (Events & FD_WRITE){
		Node->Pending = WIN_POLLOUT;	/* ssh.exe */
	}
	if (Events & FD_READ){
		sResult |= WIN_POLLIN;
	}
	if (Events & FD_CONNECT){
		sResult |= WIN_POLLWRBAND;
	}
	if (Events & FD_ACCEPT){
		Node->Pending = WIN_POLLRDBAND;	/* ftpd.exe */
		sResult |= WIN_POLLIN;
	}
	if (Events & FD_CLOSE){
		Node->Pending = WIN_POLLHUP;	/* lynx.exe */
		sResult |= WIN_POLLHUP;
	}
	/* make sure we see unhandled events */
	if (Events & ~(FD_WRITE | FD_READ | FD_CONNECT | FD_ACCEPT | FD_CLOSE)){
		WIN_ERR("[WSAPollEvents:0x%x]", Events);
	}
	return(sResult);
}
SHORT 
WSAUnblock(WIN_VNODE *Node)
{
	SHORT sResult = WIN_POLLERR;

	/* Some softwares set O_NONBLOCK after socket(), but few of them handle 
	 * EINPROGRESS during connect() or bind() (curl.exe).
	 * On the other hand, some softwares leave the socket blocking, but
	 * select() or poll() anyway (wget.exe).
	 */
	if (SOCKET_ERROR == WSAEventSelect(Node->Socket, Node->Event, FD_ALL_EVENTS)){
		WIN_ERR("WSAEventSelect(%d): %s\n", Node->Socket, win_strerror(WSAGetLastError()));
	}else{
		Node->Attribs |= FILE_FLAG_OVERLAPPED;
		sResult = 0;
	}
	return(sResult);
}

/****************************************************/

BOOL 
ws2_poll(WIN_VNODE *Node, WIN_POLLFD *Info, DWORD *Result)
{
	BOOL bResult = TRUE;
	SHORT sResult = WIN_POLLERR;
	SHORT sMask = Info->Events | WIN_POLLIGNORE;
	WSANETWORKEVENTS nwEvents = {0};
	ULONG ulCount = 0;

	if (!(Node->Attribs & FILE_FLAG_OVERLAPPED)){		/* wget.exe */
		sResult = WSAUnblock(Node);
	}else if (SOCKET_ERROR == WSAEnumNetworkEvents(Node->Socket, Node->Event, &nwEvents)){
		bResult = FALSE;
	}else if (nwEvents.lNetworkEvents){
		sResult = WSAPollEvents(Node, nwEvents.lNetworkEvents);
	}else if (!ws2_FIONREAD(Node->Socket, &ulCount)){	/* ssh.exe */
		bResult = FALSE;
	}else if (ulCount){					/* ab.exe */
		sResult = WIN_POLLIN;
	}else{
		sResult = Node->Pending;
	}
	if (Info->Result = sResult & sMask){
		*Result += 1;
	}
	return(bResult);
}
