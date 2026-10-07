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

#include <iphlpapi.h>

/****************************************************/

BOOL 
ws2_NET_RT_DUMP(PMIB_IPFORWARDTABLE *Table, PMIB_IPFORWARDROW *Row, DWORD *Count)
{
	BOOL bResult = FALSE;
	PMIB_IPFORWARDTABLE pfwTable;
	LONG lSize = 0;
	DWORD dwStatus;

	dwStatus = GetIpForwardTable(NULL, &lSize, FALSE);
	if (lSize > 0){
		pfwTable = win_malloc(lSize);
		GetIpForwardTable(pfwTable, &lSize, FALSE);
		*Table = pfwTable;
		*Row = pfwTable->table;
		*Count = pfwTable->dwNumEntries;
		bResult = TRUE;
	}else{
		WIN_ERR("GetIpForwardTable(): %s\n", win_strerror(dwStatus));
	}
	return(bResult);
}
BOOL 
ws2_NET_RT_UNKNOWN(PMIB_TCPTABLE *Table, PMIB_TCPROW *Row, DWORD *Count)
{
	BOOL bResult = FALSE;
	PMIB_TCPTABLE ptcpTable;
	LONG lSize = 0;
	DWORD dwStatus;

	dwStatus = GetTcpTable(NULL, &lSize, FALSE);
	if (lSize > 0){
		ptcpTable = win_malloc(lSize);
		GetTcpTable(ptcpTable, &lSize, FALSE);
		*Table = ptcpTable;
		*Row = ptcpTable->table;
		*Count = ptcpTable->dwNumEntries;
		bResult = TRUE;
	}else{
		WIN_ERR("GetTcpTable(): %s\n", win_strerror(dwStatus));
	}
	return(bResult);
}
BOOL 
ws2_NET_RT_OACTIVE(PMIB_IPNETTABLE *Table, PMIB_IPNETROW *Row, DWORD *Count)
{
	BOOL bResult = FALSE;
	PMIB_IPNETTABLE pinTable;
	LONG lSize = 0;
	DWORD dwStatus;
	DWORD dwCount = 0;

	dwStatus = GetIpNetTable(NULL, &lSize, FALSE);
	if (lSize > 0){
		pinTable = win_malloc(lSize);
		GetIpNetTable(pinTable, &lSize, FALSE);
		*Table = pinTable;
		*Row = pinTable->table;
		dwCount = pinTable->dwNumEntries;
		bResult = TRUE;
	}else if (dwStatus == ERROR_NO_DATA){		/* not an error */
		bResult = TRUE;
	}else{
		SetLastError(dwStatus);
	}
	*Count = dwCount;
	return(bResult);
}
BOOL 
ws2_NET_INET6_IPV6_DAD_PENDING(DWORD *Count)
{
	WIN_IFDATA ifData;
	WIN_IFADDRS ifInfo;
	PIP_ADAPTER_UNICAST_ADDRESS paUnicast;
	DWORD dwCount = 0;

	if (!ws2_setconf(&ifData, AF_INET6)){
		return(FALSE);
	}else while (ws2_getifaddrs(&ifData, &ifInfo)){
		paUnicast = ifInfo.Unicast;
		while (paUnicast){
			if (paUnicast->DadState == IpDadStateDuplicate){
				dwCount++;
			}
			paUnicast = paUnicast->Next;
		}
	}
	ws2_endconf(&ifData);
	*Count = dwCount;
	return(TRUE);
}
