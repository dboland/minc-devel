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
fifo_close(WIN_VNODE *Node)
{
	BOOL bResult = FALSE;

	if (!CloseHandle(Node->Handle)){
		WIN_ERR("fifo_close(%d): %s\n", Node->Handle, win_strerror(GetLastError()));
	}else{
		ZeroMemory(Node, sizeof(WIN_VNODE));
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
fifo_read(WIN_VNODE *Node, LPSTR Buffer, DWORD Size, DWORD *Result)
{
	BOOL bResult = FALSE;

	/* When in non-blocking mode and no data present,
	 * result should be -1 and errno EAGAIN (perl.exe).
	 */
	if (ReadFile(Node->Handle, Buffer, Size, Result, NULL)){
		return(TRUE);
	}else if (ERROR_BROKEN_PIPE == GetLastError()){	/* write end closed */
		bResult = TRUE;
	}else if (ERROR_NO_DATA == GetLastError()){
		SetLastError(ERROR_MORE_DATA);
	}
	return(bResult);
}
BOOL 
fifo_write(WIN_VNODE *Node, LPCSTR Buffer, DWORD Size, DWORD *Result)
{
	BOOL bResult = FALSE;
	OVERLAPPED ovl = {0, 0, 0, 0, Node->Event};

	/* Unlike named pipes, anonymous pipes don't block when Size
	 * is larger than WIN_PIPE_BUF.
	 */
	if (WriteFile(Node->Handle, Buffer, Size, Result, &ovl)){
		bResult = TRUE;
	}else if (ERROR_BROKEN_PIPE == GetLastError()){	/* read end closed */
		SetLastError(ERROR_PIPE_NOT_CONNECTED);
	}
	return(bResult);
}
