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

#include <sys/sockio.h>
#include <netinet6/in6_var.h>

/****************************************************/

int 
sock_SIOCGIFFLAGS(struct ifreq *req)
{
	int result = 0;
	MIB_IFROW ifInfo;
	DWORD dwStatus;

	ifInfo.dwIndex = ws2_nametoindex(req->ifr_name);
	dwStatus = GetIfEntry(&ifInfo);
	if (dwStatus != ERROR_SUCCESS){
		result -= errno_posix(dwStatus);
	}else{
		req->ifr_flags = ifflags_posix(&ifInfo);
	}
	return(result);
}
int 
sock_SIOCGIFMTU(struct ifreq *req)
{
	int result = 0;
	MIB_IFROW ifInfo;
	DWORD dwStatus;

	ifInfo.dwIndex = ws2_nametoindex(req->ifr_name);
	dwStatus = GetIfEntry(&ifInfo);
	if (dwStatus != ERROR_SUCCESS){
		result -= errno_posix(dwStatus);
	}else{
		req->ifr_mtu = ifInfo.dwMtu;
	}
	return(result);
}
int 
sock_SIOCGIFDESCR(struct ifreq *req)
{
	int result = 0;
	MIB_IFROW ifInfo;
	DWORD dwStatus;

	ifInfo.dwIndex = ws2_nametoindex(req->ifr_name);
	dwStatus = GetIfEntry(&ifInfo);
	if (dwStatus != ERROR_SUCCESS){
		result -= errno_posix(dwStatus);
	}else{
		win_strlcpy(req->ifr_data, ifInfo.bDescr, MIN(ifInfo.dwDescrLen, IFDESCRSIZE));
	}
	return(result);
}
int 
sock_SIOCGIFGROUP(struct ifgroupreq *req)
{
	char *ifname = req->ifgr_name;
	char *buf = req->ifgr_groups->ifgrq_group;
	char c;

	if (!req->ifgr_len){
		req->ifgr_len = sizeof(struct ifg_req);
	}else while (c = *ifname++){
		if (c >= '0' && c <= '9'){
			*buf = 0;
			break;
		}
		*buf++ = c;
	}
	return(0);
}
int 
sock_SIOCGIFADDR(struct ifreq *req)
{
	int result = 0;
	struct sockaddr_in *addr = (void *)&req->ifr_addr;
	MIB_IPADDRROW ifaInfo;
	DWORD dwStatus;

	ifaInfo.dwIndex = ws2_nametoindex(req->ifr_name);
	dwStatus = ws2_SIOCGIFADDR(&ifaInfo);
	if (dwStatus != ERROR_SUCCESS){
		result -= errno_posix(dwStatus);
	}else{
		inaddr_posix(addr, AF_INET, 0, (BYTE *)&ifaInfo.dwAddr);
	}
	return(result);
}
int 
sock_SIOCGIFNETMASK(struct ifreq *req)
{
	int result = 0;
	struct sockaddr_in *addr = (void *)&req->ifr_addr;
	MIB_IPADDRROW ifaInfo;
	DWORD dwStatus;

	ifaInfo.dwIndex = ws2_nametoindex(req->ifr_name);
	dwStatus = ws2_SIOCGIFADDR(&ifaInfo);
	if (dwStatus != ERROR_SUCCESS){
		result -= errno_posix(dwStatus);
	}else{
		inaddr_posix(addr, AF_INET, 0, (BYTE *)&ifaInfo.dwMask);
	}
	return(result);
}
int 
sock_SIOCGIFBRDADDR(struct ifreq *req)
{
	int result = 0;
	struct sockaddr_in *addr = (void *)&req->ifr_addr;
	DWORD dwBroadcast;
	MIB_IPADDRROW ifaInfo;
	DWORD dwStatus;

	ifaInfo.dwIndex = ws2_nametoindex(req->ifr_name);
	dwStatus = ws2_SIOCGIFADDR(&ifaInfo);
	if (dwStatus != ERROR_SUCCESS){
		result -= errno_posix(dwStatus);
	}else{
		dwBroadcast = ifaInfo.dwAddr | ~ifaInfo.dwMask;
		inaddr_posix(addr, AF_INET, 0, (BYTE *)&dwBroadcast);
	}
	return(result);
}

/****************************************************/

int 
sock_ioctl(WIN_TASK *Task, int fd, unsigned long request, va_list args)
{
	int result = 0;

	/* netintro(4)
	 */
	switch (request){
		case SIOCGIFMTU:
			result = sock_SIOCGIFMTU(va_arg(args, struct ifreq *));
			break;
		case SIOCGIFFLAGS:
			result = sock_SIOCGIFFLAGS(va_arg(args, struct ifreq *));
			break;
		case SIOCGIFGROUP:
			result = sock_SIOCGIFGROUP(va_arg(args, struct ifgroupreq *));
			break;
		case SIOCGIFDESCR:
			result = sock_SIOCGIFDESCR(va_arg(args, struct ifreq *));
			break;
		case SIOCGIFADDR:
			result = sock_SIOCGIFADDR(va_arg(args, struct ifreq *));
			break;
		case SIOCGIFNETMASK:
			result = sock_SIOCGIFNETMASK(va_arg(args, struct ifreq *));
			break;
		case SIOCGIFBRDADDR:
			result = sock_SIOCGIFBRDADDR(va_arg(args, struct ifreq *));
			break;
		case SIOCGIFNETMASK_IN6:
		case SIOCGIFAFLAG_IN6:
		case SIOCGIFALIFETIME_IN6:
			result = -EADDRNOTAVAIL;
			break;
		default:
			result = -EOPNOTSUPP;
	}
	return(result);
}
