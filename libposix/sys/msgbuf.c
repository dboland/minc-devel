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

#include <sys/msgbuf.h>

/****************************************************/

wchar_t *
cfexpand(wchar_t *string)
{
	wchar_t *result = string;
	wchar_t c;

	/* Expand Vista formatted strings by skipping them.
	 */
	while (c = *string++){
		if (c == ';' || c == '\\'){
			result = string;
		}
	}
	return(result);
}
int 
msgbuf_PDO(WIN_CFDATA *Config, WIN_CFDRIVER *Driver, LPSTR Result)
{
	LPSTR psz = Result;
	WIN_DEVICE *pwDevice = DEVICE(Driver->DeviceId);

	if (pwDevice->Flags & WIN_DVF_ACTIVE){
		psz += sprintf(psz, "%s on ", pwDevice->Name);
	}else{
		psz += sprintf(psz, "+ not configured: ");
	}
	psz += sprintf(psz, "%ls at %ls", Config->NtName, Config->BusName);
	psz += sprintf(psz, " %ls", cfexpand(Driver->Location));
	psz += sprintf(psz, ", class %ls", Driver->NtClass);
	psz += sprintf(psz, ", type 0x%x", Config->DeviceType);
	psz += sprintf(psz, ", flags 0x%x", pwDevice->Flags);
	psz += sprintf(psz, ", \"%ls\"", cfexpand(Driver->Comment));
	*psz++ = '\n';
	*psz = 0;
	return(psz - Result);
}
int 
msgbuf_DRIVE(WIN_CFDATA *Config, WIN_STATVFS *Stat, LPSTR Result)
{
	LPSTR psz = Result;
	WIN_DEVICE *pwDevice = DEVICE(Stat->DeviceId);

	psz += sprintf(psz, "%s on ", pwDevice->Name);
	psz += sprintf(psz, "%ls at %ls drive", Config->NtName, Config->BusName);
	psz += sprintf(psz, ", type 0x%x", Config->DeviceType);
	psz += sprintf(psz, ", flags 0x%x", pwDevice->Flags);
	*psz++ = '\n';
	*psz = 0;
	return(psz - Result);
}
int 
msgbuf_WINSOCK(WIN_IFDATA *Config, WIN_CFDRIVER *Driver, LPSTR Result)
{
	LPSTR psz = Result;
	WIN_DEVICE *pwDevice = DEVICE(Driver->DeviceId);

	if (pwDevice->Flags & WIN_DVF_ACTIVE){
		psz += sprintf(psz, "%s on ", pwDevice->Name);
	}else{
		psz += sprintf(psz, "+ not configured: ");
	}
	psz += sprintf(psz, "%ls", Config->AdapterName);
	psz += sprintf(psz, ", index %d", Config->IfIndex);
	psz += sprintf(psz, ", type 0x%x", Config->DeviceType);
	psz += sprintf(psz, ", flags 0x%x", pwDevice->Flags);
	psz += sprintf(psz, ", \"%ls\"", Driver->Comment);
	*psz++ = '\n';
	*psz = 0;
	return(psz - Result);
}

/****************************************************/

int 
msgbuf_KERN_MSGBUFSIZE(int *data, size_t *len)
{
	WIN_CFDATA cfData;
	WIN_CFDRIVER cfDriver;
	WIN_STATVFS fsInfo;
	DWORD dwFlags = WIN_MNT_NOWAIT;
	char *msgbuf = win_malloc(MSGBUFSIZE);
	size_t bufsize = 0;
	char *buf = msgbuf;

	if (!vfs_setconf(&cfData, dwFlags)){
		return(-ECANCELED);
	}else while (vfs_getconf(&cfData, dwFlags)){
		if (cfData.FSType == FS_TYPE_PDO){
			pdo_lookup(&cfData, dwFlags, &cfDriver);
			if (pdo_match(cfData.NtName, cfData.DeviceType, &cfDriver)){
				bufsize += msgbuf_PDO(&cfData, &cfDriver, buf);
				if (win_realloc(bufsize + MSGBUFSIZE, msgbuf, (PVOID *)&msgbuf)){
					buf = msgbuf + bufsize;
				}
			}
		}else if (cfData.FSType == FS_TYPE_DRIVE){
			drive_lookup(&cfData, dwFlags, &fsInfo);
			if (drive_match(cfData.NtName, cfData.DeviceType, &fsInfo)){
				bufsize += msgbuf_DRIVE(&cfData, &fsInfo, buf);
				if (win_realloc(bufsize + MSGBUFSIZE, msgbuf, (PVOID *)&msgbuf)){
					buf = msgbuf + bufsize;
				}
			}
		}
	}
	vfs_endconf(&cfData);
	__Globals->MsgBufSize = bufsize;
	__Globals->MsgBuffer = msgbuf;
	*data = bufsize;
	return(0);
}
int 
msgbuf_KERN_MSGBUF(char data[], size_t *len)
{
	size_t bufsize = __Globals->MsgBufSize;
	char *msgbuf = __Globals->MsgBuffer;
	struct msgbuf *msg = (struct msgbuf *)data;

	msg->msg_magic = MSG_MAGIC;
	msg->msg_bufx = bufsize;
	msg->msg_bufr = 0;
	msg->msg_bufs = bufsize;
//	msg->msg_bufl = bufsize;
	win_memcpy(&msg->msg_bufc, msgbuf, bufsize);
	win_free(msgbuf);
	return(0);
}
