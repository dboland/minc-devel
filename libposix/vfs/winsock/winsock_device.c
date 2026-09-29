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

/************************************************************/

BOOL 
ws2_setconf(WIN_IFDATA *Config)
{
	BOOL bResult = FALSE;
	ULONG ulStatus;
	PIP_ADAPTER_ADDRESSES pTable;
	LONG lSize = 0;
	ULONG ulFlags = GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_SKIP_MULTICAST;
	DWORD dwCount = 0;

	ulStatus = GetAdaptersAddresses(AF_UNSPEC, ulFlags, NULL, NULL, &lSize);
	if (lSize > 0){
		pTable = win_malloc(lSize);
		GetAdaptersAddresses(AF_UNSPEC, ulFlags, NULL, pTable, &lSize);
		Config->Table = pTable;
		Config->Next = pTable;
		Config->IfIndex = 0;
		bResult = TRUE;
	}else{
		WIN_ERR("GetAdaptersAddresses(AF_UNSPEC): %s\n", win_strerror(ulStatus));
	}
	return(bResult);
}
VOID 
ws2_endconf(WIN_IFDATA *Config)
{
	win_free(Config->Table);
}
BOOL 
ws2_getconf(WIN_IFDATA *Config, WIN_CFDRIVER *Result)
{
	BOOL bResult = FALSE;
	PIP_ADAPTER_ADDRESSES pRow = Config->Next;

	if (!pRow){
		SetLastError(ERROR_NO_MORE_ITEMS);
	}else{
		ZeroMemory(Result, sizeof(WIN_CFDRIVER));
		Config->FSType = FS_TYPE_WINSOCK;
		Config->IfIndex = pRow->IfIndex;
		Config->IfType = pRow->IfType;
		win_mbstowcs(Config->AdapterName, pRow->AdapterName, MAX_NAME);
		win_wcscpy(Result->Comment, pRow->FriendlyName);
		Config->Next = pRow->Next;
		bResult = TRUE;
	}
	return(bResult);
}

/****************************************************/

BOOL 
ws2_match(WIN_IFDATA *Config, WIN_CFDRIVER *Driver)
{
	BOOL bResult = FALSE;
	DWORD dwType = Config->DeviceType;
	WIN_DEVICE *pwDevice = DEVICE(dwType);
	USHORT sClass = dwType & 0xFF00;
	USHORT sUnit = dwType & 0x00FF;

	while (sUnit < WIN_UNIT_MAX){
		if (!wcscmp(pwDevice->NtName, Config->AdapterName)){
			bResult = TRUE;
			break;
		}else if (!pwDevice->Flags){
			pwDevice->Flags = WIN_DVF_IF_READY;
			pwDevice->DeviceType = dwType;
			pwDevice->DeviceId = sClass + sUnit;
			pwDevice->Index = Config->IfIndex;
			win_wcscpy(pwDevice->NtName, Config->AdapterName);
			win_wcscpy(pwDevice->ClassId, Driver->ClassId);
			bResult = config_attach(pwDevice, sClass);
			break;
		}
		pwDevice++;
		sUnit++;
	}
	Driver->DeviceId = pwDevice->DeviceId;
	Driver->Flags = pwDevice->Flags;
	return(bResult);
}
