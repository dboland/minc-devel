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

#include <aclapi.h>

/************************************************************/

BOOL 
AclLookup(LPCWSTR Account, SID8 *Result)
{
	BOOL bResult = FALSE;
	WCHAR szDomain[MAX_NAME];
	DWORD bufLen = MAX_NAME;
	DWORD sidLen = sizeof(SID8);
	SID_NAME_USE snType = 0;

	if (!*Account){			/* inetd.exe */
		SetLastError(ERROR_BAD_ARGUMENTS);
	}else if (!LookupAccountNameW(NULL, Account, Result, &sidLen, szDomain, &bufLen, &snType)){
		WIN_ERR("LookupAccountName(%ls): %s\n", Account, win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}
VOID 
AclInit(SID8 *SidMachine)
{
	WCHAR szName[MAX_NAME];
	DWORD dwLen = MAX_NAME;

	if (!GetComputerNameW(szName, &dwLen)){
		WIN_ERR("GetComputerName(): %s\n", win_strerror(GetLastError()));
	}else if (AclLookup(szName, SidMachine)){
		SidMachine->SubAuthority[SidMachine->SubAuthorityCount] = DOMAIN_GROUP_RID_USERS;
		SidMachine->SubAuthorityCount++;
	}
}

/************************************************************/

BOOL 
win_acl_get_file(LPCWSTR FileName, PSECURITY_DESCRIPTOR *Result)
{
	BOOL bResult = FALSE;
	LONG lSize = 0;
	SECURITY_INFORMATION siType = OWNER_SECURITY_INFORMATION
		 + GROUP_SECURITY_INFORMATION + DACL_SECURITY_INFORMATION;

	GetFileSecurityW(FileName, siType, NULL, lSize, &lSize);
	if (lSize > 0){
		*Result = LocalAlloc(LMEM_FIXED, lSize);
		bResult = GetFileSecurityW(FileName, siType, *Result, lSize, &lSize);
//	}else{
//		WIN_ERR("GetFileSecurity(%ls): %s\n", FileName, win_strerror(GetLastError()));
	}
	return(bResult);
}
BOOL 
win_acl_get_fd(HANDLE Handle, PSECURITY_DESCRIPTOR *Result)
{
	BOOL bResult = FALSE;
	LONG lSize = 0;
	SECURITY_INFORMATION siType = OWNER_SECURITY_INFORMATION
		 + GROUP_SECURITY_INFORMATION + DACL_SECURITY_INFORMATION;

	GetUserObjectSecurity(Handle, &siType, NULL, lSize, &lSize);
	if (lSize > 0){
		*Result = LocalAlloc(LMEM_FIXED, lSize);
		bResult = GetUserObjectSecurity(Handle, &siType, *Result, lSize, &lSize);
//	}else{
//		WIN_ERR("GetUserObjectSecurity(%d): %s\n", Handle, win_strerror(GetLastError()));
	}
	return(bResult);
}
BOOL 
win_acl_dup(PSECURITY_DESCRIPTOR Source, PSECURITY_DESCRIPTOR Result)
{
	BOOL bResult = FALSE;
	PSID pOwner = NULL;
	PSID pGroup = NULL;
	BOOL bOwner, bGroup;

	if (!GetSecurityDescriptorOwner(Source, &pOwner, &bOwner)){
		WIN_ERR("GetSecurityDescriptorOwner(): %s\n", win_strerror(GetLastError()));
	}
	if (!GetSecurityDescriptorGroup(Source, &pGroup, &bGroup)){
		WIN_ERR("GetSecurityDescriptorGroup(): %s\n", win_strerror(GetLastError()));
	}
	if (!InitializeSecurityDescriptor(Result, SECURITY_DESCRIPTOR_REVISION)){
		WIN_ERR("InitializeSecurityDescriptor(): %s\n", win_strerror(GetLastError()));
	}else if (!SetSecurityDescriptorOwner(Result, pOwner, bOwner)){
		WIN_ERR("SetSecurityDescriptorOwner(%s): %s\n", win_strsid(pOwner), win_strerror(GetLastError()));
	}else if (!SetSecurityDescriptorGroup(Result, pGroup, bGroup)){
		WIN_ERR("SetSecurityDescriptorGroup(%s): %s\n", win_strsid(pGroup), win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}

