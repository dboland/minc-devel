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

BOOL 
ws2_close(WIN_VNODE *Node)
{
	BOOL bResult = FALSE;

	/* closesocket() hangs on duplicates (login.exe). This occurs
	 * when other threads are doing ReadFile().
	 */
	if (!WSACloseEvent(Node->Event)){
		WIN_ERR("WSACloseEvent(%d): %s\n", Node->Event, win_strerror(WSAGetLastError()));
	}else if (SOCKET_ERROR == closesocket(Node->Socket)){
//		WIN_ERR("closesocket(%d): %s\n", Node->Socket, win_strerror(WSAGetLastError()));
		return(FALSE);
	}else{
		bResult = TRUE;
	}
	ZeroMemory(Node, sizeof(WIN_VNODE));
	return(bResult);
}
BOOL 
ws2_write(WIN_VNODE *Node, LPCSTR Buffer, DWORD Size, DWORD *Result)
{
	return(fifo_write(Node, Buffer, Size, Result));
}
BOOL 
ws2_read(WIN_TASK *Task, WIN_VNODE *Node, LPSTR Buffer, DWORD Size, DWORD *Result)
{
	BOOL bResult = FALSE;
	WIN_POLLFD fdInfo = {Node->FileId, WIN_POLLIN | WIN_POLLHUP | WIN_POLLRDBAND, 0};
	DWORD dwResult = 0;

	while (!bResult){
		if (!ws2_poll(Node, &fdInfo, &dwResult)){
			break;
		}else if (dwResult){
			bResult = fifo_read(Node, Buffer, Size, Result);
		}else if (!sock_select(Task, Node, INFINITE)){
			break;
		}
	}
	return(bResult);
}
