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

#include <ipifcons.h>

/************************************************************/

BOOL 
ws2_lookup(WIN_IFDATA *Config, WIN_IFDRIVER *Driver)
{
	BOOL bResult = TRUE;

	switch (Driver->IfType){
		case IF_TYPE_ETHERNET_CSMACD:
			Config->DeviceType = DEV_TYPE_ETH;
			break;
		case IF_TYPE_PPP:
			Config->DeviceType = DEV_TYPE_PPP;
			break;
		case IF_TYPE_SOFTWARE_LOOPBACK:
			Config->DeviceType = DEV_TYPE_LOOPBACK;
			break;
		case IF_TYPE_IEEE80211:
			Config->DeviceType = DEV_TYPE_WLAN;
			break;
		case IF_TYPE_TUNNEL:
			Config->DeviceType = DEV_TYPE_TUNNEL;
			break;
		default:
			bResult = FALSE;
	}
	return(bResult);
}
