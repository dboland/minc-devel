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

#include <net/if_dl.h>

/****************************************************/

void *
dladdr_posix(struct sockaddr_dl *addr, DWORD Index, DWORD Type, BYTE Address[], DWORD Size)
{
	char *data = addr->sdl_data;

	addr->sdl_len = sizeof(struct sockaddr_dl);
	addr->sdl_family = AF_LINK;
	addr->sdl_index = Index;
	addr->sdl_type = Type;				/* net/if_types.h */
	addr->sdl_nlen = ws2_indextoname(Index, data);	/* interface name length, no trailing 0 reqd. */
	data += addr->sdl_nlen;
	addr->sdl_alen = Size;				/* link level address length */
	win_memcpy(data, Address, Size);
	return(addr + 1);
}
u_char 
dlstate_posix(IF_OPER_STATUS OperStatus)
{
	u_char result;

	switch (OperStatus){
		IfOperStatusUp:
			result = LINK_STATE_UP;
			break;
		IfOperStatusDown:
		IfOperStatusDormant:
		IfOperStatusNotPresent:
		IfOperStatusLowerLayerDown:
			result = LINK_STATE_DOWN;
			break;
		IfOperStatusUnknown:
		IfOperStatusTesting:
		default:
			result = LINK_STATE_UNKNOWN;
			break;
	}
	return(result);
}
