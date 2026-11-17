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

#include <winbase.h>

#define WIN_RTM_GET		4

typedef struct _WIN_MSGPEEK {
	USHORT Size;
	UCHAR Version;
	UCHAR Type;
	USHORT Offset;
	USHORT Index;
	USHORT TableId;
} WIN_MSGPEEK;

static const char *__MESSAGE[] = {
	"RTM_UNKNOWN",
	"RTM_ADD",
	"RTM_DELETE",
	"RTM_CHANGE",
	"RTM_GET",
	"RTM_LOSING",
	"RTM_REDIRECT",
	"RTM_MISS",
	"RTM_LOCK",
	"RTM_RESOLVE",
	"RTM_NEWADDR",
	"RTM_DELADDR"
};

/****************************************************/

BOOL 
route_socket(INT Family, INT Type, INT Protocol, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;
	WCHAR szName[MAX_GUID];
	DWORD dwAttribs = PIPE_TYPE_BYTE | PIPE_READMODE_BYTE;

	if (Type == SOCK_DGRAM){
		dwAttribs = PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE;
	}
	if (PipeCreateFile(VfsCreateName(szName), dwAttribs, __PipeEvent, Result)){
		Result->FSType = FS_TYPE_PDO;
		Result->DeviceType = DEV_TYPE_ROUTE;
		Result->DeviceId = DEV_TYPE_ROUTE;
		bResult = TRUE;
	}
	return(TRUE);
}
BOOL 
route_read(WIN_DEVICE *Device, LPSTR Buffer, DWORD Size, DWORD *Result)
{
	SetLastError(ERROR_NOT_SUPPORTED);
	*Result = 0;
	return(FALSE);
}
BOOL 
route_write(WIN_DEVICE *Device, LPCSTR Buffer, DWORD Size, DWORD *Result)
{
	WIN_MSGPEEK *Message = (WIN_MSGPEEK *)Buffer;
	DWORD dwResult = Size;
	BOOL bResult = FALSE;

//__PRINTF("route_write(%s): Size(%d) Offset(%d) Index(%d) TableId(%d)\n", 
//	__Message[Message->Type], Message->Size, Message->Offset, Message->Index, Message->TableId)
	if (Message->Type == WIN_RTM_GET){
		dwResult = 0;
	}
	if (Message->Size == Message->Offset){
		SetLastError(ERROR_BAD_ARGUMENTS);
	}else{
		SetLastError(ERROR_NOT_SUPPORTED);
	}
	*Result = dwResult;
	return(bResult);
}
