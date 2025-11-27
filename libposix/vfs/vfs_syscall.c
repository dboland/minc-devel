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

#include <rpc.h>

typedef UCHAR *RPC_CSTR;

/****************************************************/

LPWSTR 
VfsCreateName(LPWSTR Result)
{
	UUID guid;
	RPC_CSTR pszGuid;

	if (RPC_S_OK != UuidCreate(&guid)){
		WIN_ERR("UuidCreate(): %s\n", win_strerror(GetLastError()));
	}else if (RPC_S_OK != UuidToString(&guid, &pszGuid)){
		WIN_ERR("UuidToString(): %s\n", win_strerror(GetLastError()));
	}else{
		win_mbstowcs(Result, pszGuid, MAX_GUID);
		RpcStringFree(&pszGuid);
	}
	return(Result);
}
BOOL 
VfsStatHandle(HANDLE Handle, WIN_VATTR *Result)
{
	BOOL bResult = FALSE;
	PSECURITY_DESCRIPTOR psd;

	if (!win_acl_get_fd(Handle, &psd)){
		return(FALSE);
	}else if (GetFileInformationByHandle(Handle, (BY_HANDLE_FILE_INFORMATION *)Result)){
		bResult = vfs_acl_stat(psd, Result);
	}else if (ERROR_INVALID_FUNCTION == GetLastError()){	/* Windows devices: COM, AUX (rm.exe) */
		SetLastError(ERROR_FILE_NOT_FOUND);
	}else{
		WIN_ERR("GetFileInformationByHandle(%d): %s\n", Handle, win_strerror(GetLastError()));
	}
	LocalFree(psd);
	return(bResult);
}
BOOL 
VfsStatFile(LPCWSTR FileName, DWORD Attribs, WIN_VATTR *Result)
{
	BOOL bResult = FALSE;
	HANDLE hFile;

	hFile = CreateFileW(FileName, READ_CONTROL, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, Attribs, NULL);
	if (hFile == INVALID_HANDLE_VALUE){
		return(FALSE);
	}else if (VfsStatHandle(hFile, Result)){
		bResult = CloseHandle(hFile);
	}
	return(bResult);
}
BOOL 
VfsStatNode(WIN_NAMEIDATA *Path, DWORD Flags, HANDLE *Result)
{
	BOOL bResult = FALSE;
	DWORD dwSize;
	HANDLE hResult;
	SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};
	WIN_INODE iNode;
	ACCESS_MASK amAccess = FILE_READ_DATA | READ_CONTROL;

	if (Flags & WIN_LOCKLEAF){
		amAccess |= WRITE_DAC | WRITE_OWNER;
	}
	hResult = CreateFileW(Path->Resolved, amAccess, FILE_SHARE_READ, 
		&sa, OPEN_EXISTING, FILE_CLASS_INODE, NULL);
	if (hResult == INVALID_HANDLE_VALUE){
		return(FALSE);
	}else if (!ReadFile(hResult, &iNode, sizeof(WIN_INODE), &dwSize, NULL)){
		WIN_ERR("VfsStatNode(%ls): %s\n", Path->Resolved, win_strerror(GetLastError()));
	}else if (iNode.Magic == TypeNameVirtual){
		Path->DeviceId = iNode.DeviceId;
		Path->FileType = iNode.FileType;
		Path->FSType = iNode.FSType;
		Path->Size = iNode.NameSize;
		*Result = hResult;
		bResult = TRUE;
	}else if (CloseHandle(hResult)){
		WIN_ERR("VfsStatNode(%ls): %s\n", Path->Resolved, win_strerror(ERROR_BAD_ARGUMENTS));
	}
	return(bResult);
}

/****************************************************/

BOOL 
vfs_seteuid(WIN_TASK *Task, SID8 *Sid)
{
	BOOL bResult = FALSE;
	HANDLE hToken = NULL;
	WIN_PWENT pwEntry;
	WIN_CAP_CONTROL wControl;

	if (EqualSid(Sid, &Task->UserSid)){
		return(TRUE);
	}else if (!win_getpwuid(Sid, &pwEntry)){
		return(FALSE);
	}else if (!win_cap_init(&wControl)){
		return(FALSE);
	}else if (!win_cap_setuid(&wControl, &pwEntry, &hToken)){
		bResult = FALSE;
	}else if (!SetThreadToken(NULL, hToken)){
		WIN_ERR("SetThreadToken(%ls): %s\n", pwEntry.Account, win_strerror(GetLastError()));
	}else{
		Task->UserSid = *Sid;
		bResult = CloseHandle(hToken);
	}
	win_cap_free(&wControl);
	return(bResult);
}
BOOL 
vfs_setegid(WIN_TASK *Task, SID8 *Sid)
{
	BOOL bResult = FALSE;
	HANDLE hToken = NULL;
	DWORD dwAccess = TOKEN_ADJUST_PRIVILEGES | TOKEN_ADJUST_DEFAULT;

	if (EqualSid(Sid, &Task->GroupSid)){
		return(TRUE);
	}else if (!win_cap_get_proc(dwAccess, 0, &hToken)){
		return(FALSE);
	}else if (win_cap_setgid(hToken, Sid)){
		Task->GroupSid = *Sid;
		bResult = TRUE;
	}
	CloseHandle(hToken);
	return(bResult);
}
BOOL 
vfs_getlogin(WIN_TASK *Task, LPSTR Name, DWORD Size)
{
	BOOL bResult = FALSE;
	CHAR szBuf[MAX_NAME];
	SID_NAME_USE snType = 0;
	DWORD dwSize = MAX_NAME;

	/* In WinXP, this returns logged on user, not impersonated one.
	 */
	if (!LookupAccountSid(NULL, &Task->UserSid, Name, &Size, szBuf, &dwSize, &snType)){
		WIN_ERR("LookupAccountSid(%s): %s\n", win_strsid(&Task->UserSid), win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
vfs_setlogin(WIN_TASK *Task, LPCSTR Name)
{
	BOOL bResult = FALSE;
	DWORD dwSize = sizeof(SID8);
	SID8 sid;

	if (!win_group_member(&SidAdmins)){
		SetLastError(ERROR_PRIVILEGE_NOT_HELD);
//	}else if (AclLookup(Name, &sid, &dwSize)){
	}else{
//		bResult = vfs_seteuid(Task, &sid);
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
vfs_pipe(WIN_VNODE Result[2])
{
	BOOL bResult = FALSE;
	HANDLE hInput, hOutput;

	if (!CreatePipe(&hInput, &hOutput, NULL, WIN_PIPE_BUF)){
		WIN_ERR("CreatePipe(%d): %s\n", WIN_PIPE_BUF, win_strerror(GetLastError()));
	}else{
		Result[0].Handle = hInput;
		Result[0].Event = __PipeEvent;
		Result[0].FSType = FS_TYPE_PIPE;
		Result[0].FileType = WIN_VFIFO;
		Result[0].DeviceType = DEV_CLASS_CPU;
		Result[0].Access = win_F_GETFL(hInput);
		Result[0].Attribs = FILE_ATTRIBUTE_NORMAL;
		Result[1].Handle = hOutput;
		Result[1].Event = __PipeEvent;
		Result[1].FSType = FS_TYPE_PIPE;
		Result[1].FileType = WIN_VFIFO;
		Result[1].DeviceType = DEV_CLASS_CPU;
		Result[1].Access = win_F_GETFL(hOutput);
		Result[1].Attribs = FILE_ATTRIBUTE_NORMAL;
		bResult = TRUE;
	}
	return(bResult);
}
