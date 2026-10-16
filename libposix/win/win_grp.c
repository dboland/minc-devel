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

#include <lmaccess.h>

/************************************************************/

BOOL 
GrpLookupNTAuth(WIN_GRENT *Group)
{
	BOOL bResult = TRUE;
	SID_NAME_USE snType = 0;
	SID8 Sid = {0};
	LPLOCALGROUP_MEMBERS_INFO_1 lgmInfo = NULL;
	DWORD dwCount = 0;
	DWORD dwTotal = 0;
	NET_API_STATUS naStatus;
	DWORD dwResume = 0;
	LPSTR pszMembers = Group->Members;

	naStatus = NetLocalGroupGetMembers(NULL, Group->Account, 1, (LPBYTE *)&lgmInfo, MAX_PREFERRED_LENGTH, &dwCount, &dwTotal, &dwResume);
	if (naStatus != NERR_Success){
		SetLastError(naStatus);
		bResult = FALSE;
	}else if (dwCount--){
		pszMembers = win_wcstombp(pszMembers, lgmInfo[dwCount].lgrmi1_name, MAX_NAME);
		while (dwCount--){
			pszMembers = win_stpcpy(pszMembers, ",");
			pszMembers = win_wcstombp(pszMembers, lgmInfo[dwCount].lgrmi1_name, MAX_NAME);
		}
		NetApiBufferFree(lgmInfo);
	}
	return(bResult);
}
BOOL 
GrpLookup(WIN_GRENT *Group)
{
	BOOL bResult = TRUE;
	BYTE bAuth = Group->Sid.IdentifierAuthority.Value[5];
	ULONG ulSubAuth = Group->Sid.SubAuthority[0];

	switch (ulSubAuth){
		case SECURITY_MACHINE_DOMAIN_RID:	/* 21 (Machine) */
		case SECURITY_BUILTIN_DOMAIN_RID:	/* 32 (Builtin) */
			bResult = GrpLookupNTAuth(Group);
			break;
	}
	return(bResult);
}

/************************************************************/

BOOL 
win_getgrnam(LPCWSTR Name, WIN_GRENT *Group)
{
	BOOL bResult = FALSE;
	DWORD sidLen = sizeof(SID8);
	DWORD domLen = MAX_NAME;
	SID_NAME_USE snType = 0;

	ZeroMemory(Group, sizeof(WIN_GRENT));
	if (LookupAccountNameW(NULL, Name, &Group->Sid, &sidLen, Group->Domain, &domLen, &snType)){
		win_wcscpy(Group->Account, Name);
		bResult = GrpLookup(Group);
	}
	return(bResult);
}
BOOL 
win_getgrgid(SID8 *Sid, WIN_GRENT *Group)
{
	BOOL bResult = FALSE;
	DWORD accLen = MAX_NAME;
	DWORD domLen = MAX_NAME;
	SID_NAME_USE snType = 0;

	ZeroMemory(Group, sizeof(WIN_GRENT));
	if (LookupAccountSidW(NULL, Sid, Group->Account, &accLen, Group->Domain, &domLen, &snType)){
		CopySid(GetLengthSid(Sid), &Group->Sid, Sid);
		bResult = GrpLookup(Group);
	}else{
		win_wcscpy(Group->Account, L"UNKNOWN");
	}
	return(bResult);
}
BOOL 
win_setgrent(WIN_PWENUM *Enum)
{
	BOOL bResult = TRUE;
	NET_API_STATUS naStatus;

	if (!Enum->Data){
		naStatus = NetLocalGroupEnum(NULL, 0, &Enum->Data, MAX_PREFERRED_LENGTH, &Enum->Count, &Enum->Total, &Enum->Resume);
		if (naStatus != NERR_Success){
			SetLastError(naStatus);
			bResult = FALSE;
		}
	}
	Enum->Index = 0;
	return(bResult);
}
BOOL 
win_getgrent(WIN_PWENUM *Enum, WIN_GRENT *Group)
{
	BOOL bResult = FALSE;
	DWORD dwIndex = Enum->Index;
	LPLOCALGROUP_INFO_0 plgInfo = (LPLOCALGROUP_INFO_0)Enum->Data;

	ZeroMemory(Group, sizeof(WIN_GRENT));
	if (!plgInfo){
		SetLastError(ERROR_INVALID_USER_BUFFER);
	}else if (dwIndex < Enum->Count){
		win_getgrnam(plgInfo[dwIndex].lgrpi0_name, Group);
		Enum->Index++;
		bResult = TRUE;
	}else{
		SetLastError(ERROR_NO_MORE_ITEMS);
	}
	return(bResult);
}
VOID 
win_endgrent(WIN_PWENUM *Enum)
{
	NetApiBufferFree(Enum->Data);
	Enum->Data = NULL;
	Enum->Index = 0;
	Enum->Count = 0;
	Enum->Resume = 0;
}
BOOL 
win_getgrouplist(WIN_PWENT *Passwd, SID8 *Primary, SID8 *Result[], DWORD *Count)
{
	BOOL bResult = TRUE;
	NET_API_STATUS naStatus;
	LPLOCALGROUP_USERS_INFO_0 plguInfo = NULL;
	DWORD dwCount = 0;
	DWORD dwTotal = 0;
	DWORD dwResult = 0;
	DWORD dwSize = sizeof(SID8) * 6;
	DWORD dwIndex = 0;
	SID8 *psResult = win_malloc(dwSize);

	naStatus = NetUserGetLocalGroups(NULL, Passwd->Account, 0, LG_INCLUDE_INDIRECT, (LPBYTE *)&plguInfo, MAX_PREFERRED_LENGTH, &dwCount, &dwTotal);
	if (naStatus == NERR_Success){
		dwSize += sizeof(SID8) * dwCount;
		if (!win_realloc(dwSize, psResult, (PVOID *)&psResult)){
			bResult = FALSE;
		}else while (dwIndex < dwCount){
			AclLookup(plguInfo->lgrui0_name, &psResult[dwResult]);
			if (!EqualSid(&psResult[dwResult], Primary)){
				dwResult++;
			}
			dwIndex++;
			plguInfo++;
		}
		NetApiBufferFree(plguInfo);
	}else if (naStatus != NERR_UserNotFound){
		WIN_ERR("NetUserGetLocalGroups(%ls): %s\n", Passwd->Account, win_strerror(naStatus));
		bResult = FALSE;
	}
	psResult[dwResult++] = SidInteractive;
	switch (Passwd->Privileges){
		case USER_PRIV_ADMIN:
			psResult[dwResult++] = SidAuthenticated;
			psResult[dwResult++] = SidLocalAdmin;	/* Vista */
			psResult[dwResult++] = SidIntegrityHigh;
			break;
		case USER_PRIV_USER:
			psResult[dwResult++] = SidAuthenticated;
			psResult[dwResult++] = SidLocalUser;	/* Vista */
			psResult[dwResult++] = SidIntegrityMedium;
			break;
		case USER_PRIV_GUEST:
			psResult[dwResult++] = SidUsers;	/* WS2_32.DLL/IPHLPAPI.DLL access */
			psResult[dwResult++] = SidBatch;
			psResult[dwResult++] = SidIntegrityLow;
			break;
	}
	psResult[dwResult++] = *Primary;
	*Result = psResult;
	*Count = dwResult;
	return(bResult);
}
