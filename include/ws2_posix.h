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

#include "ws2_types.h"

/* vfs_socket.c */

BOOL vfs_socket(INT Family, INT Type, INT Protocol, WIN_VNODE *Result);
BOOL vfs_connect(WIN_VNODE *Node, CONST LPSOCKADDR Address, INT Size);
BOOL vfs_bind(WIN_TASK *Task, WIN_VNODE *Node, LPSOCKADDR Address, INT Length);
BOOL vfs_accept(WIN_TASK *Task, WIN_VNODE *Node, LPSOCKADDR Address, LPINT Length, WIN_VNODE *Result);
BOOL vfs_sendto(WIN_VNODE *Node, LPCSTR Buffer, UINT Size, DWORD Flags, CONST LPSOCKADDR To, UINT ToLen, DWORD *Result);
BOOL vfs_recvfrom(WIN_VNODE *Node, LPSTR Buffer, UINT Size, DWORD Flags, LPSOCKADDR From, UINT *FromLen, DWORD *Result);
BOOL vfs_sendmsg(WIN_VNODE *Node, WSAMSG *Message, DWORD *Result);
BOOL vfs_recvmsg(WIN_VNODE *Node, WSAMSG *Message, DWORD *Result);
BOOL vfs_shutdown(WIN_VNODE *Node, INT How);
BOOL vfs_socketpair(INT Family, DWORD Mode, INT Protocol, WIN_VNODE Result[2]);
BOOL vfs_getsockopt(WIN_VNODE *Node, INT Level, INT Name, CHAR *Value, INT *Length);
BOOL vfs_setsockopt(WIN_VNODE *Node, INT Level, INT Name, CONST CHAR *Value, INT Length);
BOOL vfs_listen(WIN_VNODE *Node, INT Backlog);
BOOL vfs_getpeername(WIN_VNODE *Node, LPSOCKADDR Name, INT *Length);
BOOL vfs_getsockname(WIN_VNODE *Node, LPSOCKADDR Name, INT *Length);

/* winsock_device.c */

BOOL ws2_setconf(WIN_IFDATA *Config, ULONG Family);
VOID ws2_endconf(WIN_IFDATA *Config);
BOOL ws2_getconf(WIN_IFDATA *Config, WIN_IFDRIVER *Result);
BOOL ws2_match(WIN_IFDATA *Config, WIN_IFDRIVER *Driver);

/* winsock_sysctl.c */

BOOL ws2_NET_RT_DUMP(PMIB_IPFORWARDTABLE *Table, PMIB_IPFORWARDROW *Row, DWORD *Count);
BOOL ws2_NET_RT_OACTIVE(PMIB_IPNETTABLE *Table, PMIB_IPNETROW *Row, DWORD *Count);
BOOL ws2_NET_INET6_IPV6_DAD_PENDING(DWORD *Count);

/* winsock_namei.c */

BOOL ws2_lookup(WIN_IFDATA *Config, WIN_IFDRIVER *Driver);

/* winsock_ifaddrs.c */

BOOL ws2_getifaddrs(WIN_IFDATA *Config, WIN_IFADDRS *Result);

/* winsock_sockio.c */

DWORD ws2_SIOCGIFADDR(MIB_IPADDRROW *Address);

