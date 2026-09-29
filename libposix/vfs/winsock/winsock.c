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

#include "winsock_syscall.c"
#include "winsock_device.c"
#include "winsock_filio.c"
#include "winsock_fcntl.c"
#include "winsock_poll.c"
#include "winsock_socket.c"
#include "winsock_sockio.c"
#include "winsock_unistd.c"
#include "winsock_namei.c"
#include "winsock_stat.c"
#include "winsock_if.c"
#include "winsock_ifaddrs.c"
#include "winsock_sysctl.c"

/************************************************************/

BOOL 
ws2_start(VOID)
{
	BOOL bResult = FALSE;
	WSADATA wsaData;
	WORD wVersion = 0x0202;		/* v. 2.2 */

	if (WSAStartup(wVersion, &wsaData)){
		WIN_ERR("WSAStartup(0x%x): %s\n", wVersion, win_strerror(WSAGetLastError()));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
ws2_finish(VOID)
{
	BOOL bResult = FALSE;

	if (WSACleanup()){
		WIN_ERR("WSACleanup(): %s\n", win_strerror(WSAGetLastError()));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}
