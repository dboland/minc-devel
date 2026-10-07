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

#include <iprtrmib.h>

/****************************************************/

DWORD 
WSALookupFlags(PIP_ADAPTER_ADDRESSES Adapter)
{
	DWORD dwResult = WS_IFF_UP;

	if (Adapter->OperStatus == IfOperStatusUp){
		dwResult |= WS_IFF_RUNNING;
	}
	if (!(Adapter->Flags & IP_ADAPTER_NO_MULTICAST)){
		dwResult |= WS_IFF_MULTICAST;
	}
	switch (Adapter->IfType){
		case IF_TYPE_PPP:
			dwResult |= WS_IFF_POINTOPOINT;
			break;
		case IF_TYPE_SOFTWARE_LOOPBACK:
			dwResult |= WS_IFF_LOOPBACK;
			break;
		case IF_TYPE_ETHERNET_CSMACD:
		case IF_TYPE_IEEE80211:
			dwResult |= WS_IFF_BROADCAST;
			break;
	}
	return(dwResult);
}

/****************************************************/

BOOL 
ws2_getifaddrs(WIN_IFDATA *Config, WIN_IFADDRS *Result)
{
	BOOL bResult = FALSE;
	PIP_ADAPTER_ADDRESSES pAdapter = Config->Next;

	if (!pAdapter){
		SetLastError(ERROR_NO_MORE_ITEMS);
	}else{
		Result->OperStatus = pAdapter->OperStatus;
		Result->IfIndex = pAdapter->IfIndex;
		Result->IfType = pAdapter->IfType;
		Result->IfFlags = WSALookupFlags(pAdapter);
		Result->Mtu = pAdapter->Mtu;
		Result->Unicast = pAdapter->FirstUnicastAddress;
		Result->AddrLen = pAdapter->PhysicalAddressLength;
		win_memcpy(Result->PhysAddr, pAdapter->PhysicalAddress, Result->AddrLen);
		Config->Next = pAdapter->Next;
		bResult = TRUE;
	}
	return(bResult);
}

