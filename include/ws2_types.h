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

#include "win/ws2ipdef.h"

#define WS_IFF_UP          0x1             /* interface is up */
#define WS_IFF_BROADCAST   0x2             /* broadcast address valid */
#define WS_IFF_DEBUG       0x4             /* turn on debugging */
#define WS_IFF_LOOPBACK    0x8             /* is a loopback net */
#define WS_IFF_POINTOPOINT 0x10            /* interface is point-to-point link */
#define WS_IFF_NOTRAILERS  0x20            /* avoid use of trailers */
#define WS_IFF_RUNNING     0x40            /* resources allocated */
#define WS_IFF_NOARP       0x80            /* no address resolution protocol */
#define WS_IFF_PROMISC     0x100           /* receive all packets */
#define WS_IFF_ALLMULTI    0x200           /* receive all multicast packets */
#define WS_IFF_OACTIVE     0x400           /* transmission in progress */
#define WS_IFF_SIMPLEX     0x800           /* can't hear own transmissions */
#define WS_IFF_LINK0       0x1000          /* per link layer defined bit */
#define WS_IFF_LINK1       0x2000          /* per link layer defined bit */
#define WS_IFF_LINK2       0x4000          /* per link layer defined bit */
#define WS_IFF_MULTICAST   0x8000          /* supports multicast */

/* winsock.h */

#define WS_MSG_NOSIGNAL	0x400		/* do not send SIGPIPE */

/* winsock2.h */

#define WS_AF_UNSPEC	AF_UNSPEC
#define WS_AF_LOCAL	AF_UNIX
#define WS_AF_INET	AF_INET
#define WS_AF_INET6	23
#define WS_AF_ROUTE	24

/*
 * winsock_device.c
 */

typedef struct _WIN_IFDATA {
	PVOID Table;
	PIP_ADAPTER_ADDRESSES Next;
	DWORD IfIndex;
	DWORD IfType;
	DWORD FSType;
	DWORD DeviceType;
	WCHAR AdapterName[MAX_NAME];
} WIN_IFDATA;

#define WS_SOCKET_SIZE		0x2000

/*
 * winsock_if.c
 */

typedef struct _WIN_IFENUM {
	PVOID Table;
	PIP_ADAPTER_ADDRESSES Next;
} WIN_IFENUM;

typedef struct _WIN_IFENT {
	DWORD IfIndex;
	DWORD IfType;
	DWORD IfFlags;
	DWORD Mtu;
	PIP_ADAPTER_UNICAST_ADDRESS Unicast;
	DWORD AddrLen;
	BYTE PhysAddr[MAXLEN_PHYSADDR];
} WIN_IFENT;

