/*
 * Copyright (c) 2026 Daniel Boland <dboland@xs4all.nl>.
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

#include <net/if_media.h>

/****************************************************/

int 
ifmedia_ETHER(ULONG Speed)
{
	int result = IFM_ETHER;

	/* FDX: Full duplex */
	if (Speed >= 1000){
		result |= IFM_1000_TX | IFM_AUTO | IFM_FDX;
	}else if (Speed >= 100){
		result |= IFM_100_TX | IFM_AUTO | IFM_FDX;
	}else{
		result |= IFM_10_T | IFM_AUTO;
	}
	return(result);
}
int 
ifmedia_IEEE80211(ULONG Speed)
{
	int result = IFM_IEEE80211;

	if (Speed >= 72){
		result |= IFM_IEEE80211_OFDM72;
	}else if (Speed >= 54){
		result |= IFM_IEEE80211_OFDM54;
	}else if (Speed >= 48){
		result |= IFM_IEEE80211_OFDM48;
	}else if (Speed >= 36){
		result |= IFM_IEEE80211_OFDM36;
	}else if (Speed >= 24){
		result |= IFM_IEEE80211_OFDM24;
	}else if (Speed >= 18){
		result |= IFM_IEEE80211_OFDM18;
	}else if (Speed >= 12){
		result |= IFM_IEEE80211_OFDM12;
	}
	return(result);
}
int 
ifmedia_posix(struct ifmediareq *req, MIB_IFROW *Adapter)
{
	int result = 0;
	ULONG ulSpeed = Adapter->dwSpeed * 0.000001;	/* megabit per second */

	req->ifm_status = IFM_AVALID;
	if (Adapter->dwOperStatus == IF_OPER_STATUS_OPERATIONAL){
		req->ifm_status |= IFM_ACTIVE;
	}
	switch (Adapter->dwType){
		case IF_TYPE_ETHERNET_CSMACD:
			req->ifm_current = ifmedia_ETHER(ulSpeed);
			break;
		case IF_TYPE_IEEE80211:
			req->ifm_current = ifmedia_IEEE80211(ulSpeed);
			break;
		default:
			result = -EADDRNOTAVAIL;
	}
	return(result);
}
int 
ifmcount_posix(struct ifmediareq *req, MIB_IFROW *Adapter)
{
	int result = 0;

	switch (Adapter->dwType){
		case IF_TYPE_ETHERNET_CSMACD:
			req->ifm_count = 1;
			break;
		case IF_TYPE_IEEE80211:
			req->ifm_count = 1;
			break;
		default:
			result = -EADDRNOTAVAIL;
	}
	return(result);
}

/****************************************************/

int 
sock_SIOCGIFMEDIA(struct ifmediareq *req)
{
	int result = 0;
	MIB_IFROW ifInfo;
	DWORD dwStatus;

	/* ifmedia(4)
	 */
	ifInfo.dwIndex = ws2_nametoindex(req->ifm_name);
	dwStatus = GetIfEntry(&ifInfo);
	if (dwStatus != ERROR_SUCCESS){
		result -= errno_posix(dwStatus);
	}else if (!req->ifm_ulist){
		result = ifmcount_posix(req, &ifInfo);
	}else{
		result = ifmedia_posix(req, &ifInfo);
	}
	return(result);
}
