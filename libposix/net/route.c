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

#include <net/route.h>

#define IFMSGLEN	(sizeof(struct if_msghdr) + sizeof(struct sockaddr_dl))
#define IFAMSGLEN	(sizeof(struct ifa_msghdr) + (sizeof(struct sockaddr_in) * 3))
#define RTMSGLEN	(sizeof(struct rt_msghdr) + (sizeof(struct sockaddr_in) * 2))
#define INMSGLEN	(sizeof(struct ifa_msghdr) + sizeof(struct sockaddr_in) + sizeof(struct sockaddr_dl))

/****************************************************/

void *
ifamsg_posix(WIN_TASK *Task, void *buf, WIN_IFENT *Adapter, PSOCKET_ADDRESS Address)
{
	struct ifa_msghdr *hdr = buf;
	UINT uiLength = Address->iSockaddrLength;

	hdr->ifam_msglen = sizeof(struct ifa_msghdr) + uiLength;
	hdr->ifam_version = RTM_VERSION;
	hdr->ifam_type = RTM_NEWADDR;
	hdr->ifam_hdrlen = sizeof(struct ifa_msghdr);
	hdr->ifam_index = Adapter->IfIndex;
//	hdr->ifam_tableid
	/* records added below need to be announced here */
	hdr->ifam_addrs = RTA_IFA;
	hdr->ifam_flags = Adapter->IfFlags;
//	hdr->ifam_metric
	buf += hdr->ifam_hdrlen;
	saddr_posix(Task, buf, Address->lpSockaddr, &uiLength);
	return(buf + uiLength);
}
void *
ifmsg_posix(WIN_TASK *Task, void *buf, WIN_IFENT *Adapter)
{
	struct if_msghdr *hdr = buf;
	struct if_data *data = &hdr->ifm_data;
	MIB_IFROW ifRow = {0};
	PIP_ADAPTER_UNICAST_ADDRESS paUnicast = Adapter->Unicast;

	hdr->ifm_msglen = IFMSGLEN;
	hdr->ifm_version = RTM_VERSION;
	hdr->ifm_type = RTM_IFINFO;
	hdr->ifm_hdrlen = sizeof(struct if_msghdr);
	hdr->ifm_index = Adapter->IfIndex;
//	hdr->ifm_tableid
	/* announce type of records added below */
	hdr->ifm_addrs = RTA_IFP | RTA_IFA;
	hdr->ifm_flags = Adapter->IfFlags;
//	hdr->ifm_xflags
	data->ifi_type = Adapter->IfType;
	data->ifi_addrlen = sizeof(struct sockaddr_dl);
	data->ifi_hdrlen = sizeof(struct if_data);
//	data->ifi_link_state
	data->ifi_mtu = Adapter->Mtu;
//	data->ifi_metric
	ifRow.dwIndex = Adapter->IfIndex;
	if (ERROR_SUCCESS == GetIfEntry(&ifRow)){
		data->ifi_baudrate = ifRow.dwSpeed;
		data->ifi_ipackets = ifRow.dwInUcastPkts;
		data->ifi_ierrors = ifRow.dwInErrors;
		data->ifi_opackets = ifRow.dwOutUcastPkts;
		data->ifi_oerrors = ifRow.dwOutErrors;
//		data->ifi_collisions
		data->ifi_ibytes = ifRow.dwInOctets;
		data->ifi_obytes = ifRow.dwOutOctets;
	}
//	data->ifi_imcasts
//	data->ifi_omcasts
//	data->ifi_iqdrops
//	data->ifi_noproto
//	data->ifi_capabilities
//	data->ifi_lastchange
//	data->ifi_mclpool
	buf += hdr->ifm_hdrlen;
	buf = dladdr_posix(buf, Adapter->IfIndex, Adapter->IfType, Adapter->PhysAddr, Adapter->AddrLen);
	while (paUnicast){
		buf = ifamsg_posix(Task, buf, Adapter, &paUnicast->Address);
		paUnicast = paUnicast->Next;
	}
	return(buf);
}
void *
rtmsg_posix(void *buf, MIB_IFROW *Interface, MIB_IPFORWARDROW *Address)
{
	struct rt_msghdr *hdr = buf;
	struct rt_metrics *metrics = &hdr->rtm_rmx;
	struct sockaddr_in *addr;

	hdr->rtm_msglen = RTMSGLEN;
	hdr->rtm_version = RTM_VERSION;
	hdr->rtm_type = RTM_NEWADDR;
	hdr->rtm_hdrlen = sizeof(struct rt_msghdr);
	hdr->rtm_index = Address->dwForwardIfIndex;
//	hdr->rtm_tableid
//	hdr->rtm_priority
//	hdr->rtm_mpls
	/* address types added below */
	hdr->rtm_addrs = RTA_DST | RTA_GATEWAY;
	hdr->rtm_flags = ifflags_posix(Interface);
//	hdr->rtm_fmask
//	hdr->rtm_pid
//	hdr->rtm_seq
//	hdr->rtm_errno
//	hdr->rtm_inits
//	metrics->rmx_pksent
//	metrics->rmx_expire
//	metrics->rmx_locks
	metrics->rmx_mtu = Interface->dwMtu;
//	metrics->rmx_refcnt
//	metrics->rmx_hopcount
//	metrics->rmx_recvpipe
//	metrics->rmx_sendpipe
//	metrics->rmx_ssthresh
//	metrics->rmx_rtt
//	metrics->rmx_rttvar
	addr = buf + hdr->rtm_hdrlen;
	inaddr_posix(addr++, AF_INET, 0, (BYTE *)&Address->dwForwardDest);
	inaddr_posix(addr++, AF_INET, 0, (BYTE *)&Address->dwForwardNextHop);
	return(addr);
}
void *
inmsg_posix(void *buf, MIB_IPNETROW *Address)
{
	struct ifa_msghdr *hdr = buf;

	hdr->ifam_msglen = INMSGLEN;
	hdr->ifam_version = RTM_VERSION;
	hdr->ifam_type = RTM_NEWADDR;
	hdr->ifam_hdrlen = sizeof(struct ifa_msghdr);
	hdr->ifam_index = Address->dwIndex;
//	hdr->ifam_tableid
//	hdr->ifam_addrs = RTA_IFP | RTA_IFA | RTA_DST;
	hdr->ifam_addrs = RTA_IFA | RTA_DST;
	if (Address->dwType == MIB_IPNET_TYPE_DYNAMIC){
		hdr->ifam_flags = IFA_ROUTE;
	}
//	hdr->ifam_metric
	buf += hdr->ifam_hdrlen;
	buf = inaddr_posix(buf, AF_INET, 0, (BYTE *)&Address->dwAddr);
	buf = dladdr_posix(buf, Address->dwIndex, Address->dwType, Address->bPhysAddr, Address->dwPhysAddrLen);
	return(buf);
}
/* void 
rtaddr_posix(struct sockaddr_in *addr, DWORD Port, BYTE Address[4])
{
//	if (Row->dwForwardType == MIB_IPROUTE_TYPE_INDIRECT){

	addr->sin_len = sizeof(struct sockaddr_in);
	addr->sin_family = AF_ROUTE;
	addr->sin_port = Port;
	win_memcpy(&addr->sin_addr, Address, 4);
} */
size_t 
ifmsg_size(PIP_ADAPTER_UNICAST_ADDRESS Unicast)
{
	size_t result = IFMSGLEN;

	while (Unicast){
		result += sizeof(struct ifa_msghdr);
		result += Unicast->Address.iSockaddrLength;
		Unicast = Unicast->Next;
	}
	return(result);
}

/****************************************************/

int 
route_NET_RT_IFLIST(void *buf, size_t *size)
{
	int result = 0;
	WIN_IFENUM ifEnum;
	WIN_IFENT ifInfo;
	WIN_TASK *pwTask = &__Tasks[CURRENT];

	if (!buf){
		*size = 0;
	}else{
		win_bzero(buf, *size);
	}
	if (!ws2_setifaddrs(WS_AF_UNSPEC, &ifEnum)){
		result -= errno_posix(GetLastError());
	}else while (ws2_getifaddrs(&ifEnum, &ifInfo)){
		if (!buf){
			*size += ifmsg_size(ifInfo.Unicast);
		}else{
			buf = ifmsg_posix(pwTask, buf, &ifInfo);
		}
	}
	ws2_endifaddrs(&ifEnum);
	return(result);
}
int 
route_NET_RT_OACTIVE(void *buf, size_t *size)
{
	int result = 0;
	PMIB_IPNETTABLE pinTable;
	PMIB_IPNETROW pinRow;
	DWORD dwCount;

	if (!ws2_NET_RT_OACTIVE(&pinTable, &pinRow, &dwCount)){
		result -= errno_posix(GetLastError());
	}else if (!buf){
		*size = INMSGLEN * dwCount;
	}else while (dwCount--){
		buf = inmsg_posix(buf, pinRow);
		pinRow++;
	}
	win_free(pinTable);
	return(result);
}
int 
route_NET_RT_DUMP(void *buf, size_t *size)
{
	int result = 0;
	PMIB_IPFORWARDTABLE pfwTable;
	PMIB_IPFORWARDROW pfwRow;
	DWORD dwCount = 0;
	MIB_IFROW ifRow;
	DWORD dwResult;

	if (buf){
		win_bzero(buf, *size);
	}
	if (!ws2_NET_RT_DUMP(&pfwTable, &pfwRow, &dwCount)){
		result -= errno_posix(GetLastError());
	}else if (!buf){
		*size = RTMSGLEN * dwCount;
	}else while (dwCount--){
		ifRow.dwIndex = pfwRow->dwForwardIfIndex;
		dwResult = GetIfEntry(&ifRow);
		if (dwResult == ERROR_SUCCESS){
			buf = rtmsg_posix(buf, &ifRow, pfwRow);
		}else{
			result -= errno_posix(dwResult);
			break;
		}
		pfwRow++;
	}
	win_free(pfwTable);
	return(result);
}
int 
route_NET_RT_FLAGS(const int *name, void *buf, size_t *size)
{
	int result = 0;

	switch (name[5]){
		case IFF_OACTIVE:	/* arp.exe -a */
			result = route_NET_RT_OACTIVE(buf, size);
			break;
		default:
			result = -ENOENT;
	}
	return(result);
}
