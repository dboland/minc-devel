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

#define NDIS_LAN_CLASS		L"{ad498944-762f-11d0-8dcb-00c04fc3358c}"

/****************************************************/

BOOL 
ws2_match(WIN_IFDATA *Config, WIN_IFDRIVER *Driver)
{
	BOOL bResult = FALSE;
	DWORD dwDeviceType = Config->DeviceType;
	WIN_DEVICE *pwDevice = DEVICE(dwDeviceType);
	USHORT sClass = dwDeviceType & 0xFF00;
	USHORT sUnit = dwDeviceType & 0x00FF;

	while (sUnit < WIN_UNIT_MAX){
		if (!wcscmp(pwDevice->NtName, Driver->AdapterName)){
			bResult = TRUE;
			break;
		}else if (!pwDevice->Flags){
			pwDevice->Flags = WIN_DVF_WINSOCK_READY;
			pwDevice->DeviceType = dwDeviceType;
			pwDevice->DeviceId = sClass + sUnit;
			pwDevice->Index = Driver->IfIndex;
			win_wcscpy(pwDevice->NtName, Driver->AdapterName);
			win_wcscpy(pwDevice->ClassId, NDIS_LAN_CLASS);
			bResult = config_attach(pwDevice, sClass);
			break;
		}
		pwDevice++;
		sUnit++;
	}
	Driver->DeviceId = pwDevice->DeviceId;
	Config->Flags = pwDevice->Flags;
	return(bResult);
}
