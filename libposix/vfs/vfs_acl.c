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

/* ON ACL INHERITANCE
 *
 * Windows access rights inheritance and propagation can be achieved
 * in multiple ways with multiple effects.
 *
 * 1. By hand using the GUI via the Properties/Security tab
 * 2. By hand using the CLI via the cacls command:
 *    the new AC entry is placed at the end of the AC list
 * 3. Automatically using the API via AceFlags
 * 4. Fully automatic using the Inheritance feature in the OS
 *
 */

/************************************************************/

DWORD 
AclOwnerType(ACE_PEEK *Entry, DWORD *TypeMask)
{
	DWORD dwResult = 0;
	ACCESS_MASK aMask = Entry->Mask;
	DWORD dwTypeMask = *TypeMask;
	BYTE bFlags = Entry->Header.AceFlags;

	if (Entry->Header.AceType != ACCESS_ALLOWED_ACE_TYPE){
		return(0);
	}else if (bFlags & INHERIT_ONLY_ACE){
		return(WIN_ACL_TYPE_DEFAULT);
	}else if (aMask & WRITE_DAC){
		if (dwTypeMask & WIN_ACL_USER){
			dwResult = WIN_ACL_GROUP;
		}else{
			dwResult = WIN_ACL_USER;
		}
	}else if (aMask & DELETE){			/* Vista */
		if (dwTypeMask & WIN_ACL_GROUP){
			dwResult = WIN_ACL_OTHER;
		}else{
			dwResult = WIN_ACL_GROUP;
		}
	}else if (aMask & READ_CONTROL){
		if (dwTypeMask & WIN_ACL_OTHER){
			dwResult = WIN_ACL_GROUP;
		}else{
			dwResult = WIN_ACL_OTHER;
		}
	}
	if (dwTypeMask & dwResult){
		dwResult = WIN_ACL_TYPE_ACCESS;
	}else{
		*TypeMask |= dwResult;
	}
	return(dwResult);
}
ACE_PEEK *
AclDupEntry(ACE_PEEK *Entry, PSID Sid, ACCESS_ALLOWED_ACE8 *Result)
{
	WORD wSidSize = GetLengthSid(Sid);
	WORD wAceSize = sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD) + wSidSize;

	Result->Header.AceType = Entry->Header.AceType;
	Result->Header.AceFlags = Entry->Header.AceFlags;
	Result->Header.AceSize = wAceSize;
	CopySid(wSidSize, &Result->Sid, Sid);
	return((ACE_PEEK *)Result);
}
BOOL 
AclCreateControl(DWORD Group, DWORD Other, WIN_OBJECT_CONTROL *Result)
{
	BOOL bResult = FALSE;
	PSECURITY_DESCRIPTOR pSec = &Result->Security;
	PACL pAcl = &Result->DefaultAcl;

	if (!win_cap_set_mode(Group, Other, pAcl)){
		return(FALSE);
	}else if (!InitializeSecurityDescriptor(pSec, SECURITY_DESCRIPTOR_REVISION)){
		WIN_ERR("InitializeSecurityDescriptor(): %s\n", win_strerror(GetLastError()));
	}else if (!SetSecurityDescriptorDacl(pSec, TRUE, pAcl, FALSE)){
		WIN_ERR("SetSecurityDescriptorDacl(): %s\n", win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
AclAddEntry(PACL Acl, BYTE Flags, ACCESS_MASK Access, PSID Sid)
{
	WORD wSidSize = GetLengthSid(Sid);
	WORD wAceSize = ACL_ACESIZE + wSidSize;
	ACCESS_ALLOWED_ACE8 acEntry = {
		{ACCESS_ALLOWED_ACE_TYPE, INHERITED_ACE | Flags, wAceSize}, Access
	};

	/* When adding ACE, Flags should include INHERITED_ACE.
	 */
	CopySid(wSidSize, &acEntry.Sid, Sid);
	Acl->AclSize += wAceSize;
	win_realloc(Acl->AclSize, Acl, (PVOID *)&Acl);
	return(AddAce(Acl, ACL_REVISION, MAXDWORD, &acEntry, wAceSize));
}
BOOL 
AclSidIsNone(SID8 *Sid)
{
	BOOL bResult = FALSE;

	if (Sid->SubAuthority[Sid->SubAuthorityCount-1] == DOMAIN_GROUP_RID_USERS){
		bResult = TRUE;
	}
	return(bResult);
}

/************************************************************/

BOOL 
vfs_acl_init(WIN_ACL_CONTROL *Control, DWORD MountId, DWORD Mode, PSECURITY_DESCRIPTOR Result)
{
	BOOL bResult = FALSE;
	PSID pOwner = NULL;
	PSID pGroup = NULL;
	BOOL bOwner, bGroup;
	DWORD dwType = __Mounts[MountId].DeviceType;

	if (!GetSecurityDescriptorOwner(Control->Source, &pOwner, &bOwner)){
		WIN_ERR("GetSecurityDescriptorOwner(): %s\n", win_strerror(GetLastError()));
	}
	if (!GetSecurityDescriptorGroup(Control->Source, &pGroup, &bGroup)){
		WIN_ERR("GetSecurityDescriptorGroup(): %s\n", win_strerror(GetLastError()));
	}
	if (Mode & WIN_S_ISUID){
		pOwner = &SidAdmins;
	}else if (dwType != DEV_TYPE_REMOTE){	/* Samba */
		pOwner = Control->Owner;
	}
	if (Mode & WIN_S_ISGID){
		pGroup = &SidSystem;
	}else if (dwType != DEV_TYPE_REMOTE){
		pGroup = Control->Group;
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
VOID 
vfs_acl_free(WIN_ACL_CONTROL *Control)
{
	win_free(Control->Acl);
	LocalFree(Control->Source);
}
BOOL 
vfs_acl_create(WIN_ACL_CONTROL *Control, WIN_MODE *Mode, BYTE Flags, PSECURITY_DESCRIPTOR Result)
{
	BOOL bResult = FALSE;
	ACE_PEEK *pEntry;
	WORD wIndex = 0;
	PACL paclNew, pAcl = NULL;
	ACCESS_MASK aMaskNew;
	ACCESS_ALLOWED_ACE8 aceBuf;
	BOOL bDefaulted, bPresent;
	BYTE bFlagsNew;
	DWORD dwType, dwTypeMask = 0;

	if (!GetSecurityDescriptorDacl(Control->Source, &bPresent, &pAcl, &bDefaulted)){
		WIN_ERR("GetSecurityDescriptorDacl(): %s\n", win_strerror(GetLastError()));
	}else if (bPresent && pAcl){
		wIndex = pAcl->AceCount;
	}
	paclNew = win_malloc(sizeof(ACL));
	paclNew->AclRevision = ACL_REVISION;
	paclNew->AclSize = sizeof(ACL);
	paclNew->AceCount = 0;
	while (wIndex--){
		GetAce(pAcl, wIndex, (PVOID)&pEntry);
		dwType = AclOwnerType(pEntry, &dwTypeMask);
		aMaskNew = pEntry->Mask & ~WIN_S_IRWX;
		bFlagsNew = pEntry->Header.AceFlags & ~ALL_INHERIT_ACE;
		if (dwType == WIN_ACL_USER){
			if (EqualPrefixSid(&pEntry->Sid, __SidMachine)){
				pEntry = AclDupEntry(pEntry, Control->Owner, &aceBuf);
			}
			pEntry->Mask = aMaskNew + Mode->User;
		}else if (dwType == WIN_ACL_GROUP){
			pEntry->Mask = aMaskNew + Mode->Group;
		}else if (dwType == WIN_ACL_OTHER){
			pEntry->Mask = aMaskNew + Mode->Other;
		}else if (dwType != WIN_ACL_TYPE_ACCESS){
			continue;
		}
		pEntry->Header.AceFlags = bFlagsNew + Flags;
		paclNew->AclSize += pEntry->Header.AceSize;
		if (win_realloc(paclNew->AclSize, paclNew, (PVOID *)&paclNew)){
			AddAce(paclNew, ACL_REVISION, 0, pEntry, pEntry->Header.AceSize);
		}
	}
	/* When creating files/subfolders in in user's profile tree (git.exe)
	 */
	if (!(dwTypeMask & WIN_ACL_OTHER)){
		aMaskNew = READ_CONTROL | SYNCHRONIZE;
		aMaskNew |= FILE_READ_ATTRIBUTES | FILE_READ_EA | Mode->Other;
		AclAddEntry(paclNew, Flags, aMaskNew, &SidAuthenticated);
	}
	if (!SetSecurityDescriptorDacl(Result, bPresent, paclNew, FALSE)){
		WIN_ERR("SetSecurityDescriptorDacl(): %s\n", win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	Control->Acl = paclNew;
	return(bResult);
}
BOOL 
vfs_acl_chmod(WIN_ACL_CONTROL *Control, WIN_MODE *Mode, PSECURITY_DESCRIPTOR Result)
{
	BOOL bResult = FALSE;
	ACE_PEEK *Info;
	WORD wIndex = 0;
	ACCESS_MASK amNew;
	BOOL bPresent = FALSE;
	BOOL bDefaulted = FALSE;
	PACL pAcl = NULL;
	DWORD dwType, dwTypeMask = 0;

	if (!GetSecurityDescriptorDacl(Control->Source, &bPresent, &pAcl, &bDefaulted)){
		WIN_ERR("GetSecurityDescriptorDacl(): %s\n", win_strerror(GetLastError()));
	}else if (bPresent && pAcl){
		wIndex = pAcl->AceCount;
	}
	while (wIndex--){
		GetAce(pAcl, wIndex, (PVOID)&Info);
		amNew = Info->Mask & ~WIN_S_IRWX;
		dwType = AclOwnerType(Info, &dwTypeMask);
		if (dwType == WIN_ACL_USER){
			Info->Mask = amNew + Mode->User;
		}else if (dwType == WIN_ACL_GROUP){
			Info->Mask = amNew + Mode->Group;
		}else if (dwType == WIN_ACL_OTHER){
			Info->Mask = amNew + Mode->Other;
		}
	}
	if (!SetSecurityDescriptorDacl(Result, bPresent, pAcl, bDefaulted)){
		WIN_ERR("SetSecurityDescriptorDacl(): %s\n", win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
vfs_acl_chown(WIN_ACL_CONTROL *Control, PSID NewUser, PSID NewGroup, PSECURITY_DESCRIPTOR Result)
{
	BOOL bResult = FALSE;
	BOOL bPresent = FALSE;
	BOOL bDefaulted;
	PACL paclNew, pAcl = NULL;
	ACE_PEEK *pEntry;
	WORD wIndex = 0;
	DWORD sdSize;
	ACCESS_ALLOWED_ACE8 aceBuf;
	WORD aceSize = sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD);
	DWORD dwType, dwTypeMask = 0;

	if (!GetSecurityDescriptorDacl(Control->Source, &bPresent, &pAcl, &bDefaulted)){
		WIN_ERR("GetSecurityDescriptorDacl(): %s\n", win_strerror(GetLastError()));
	}else if (bPresent && pAcl){
		wIndex = pAcl->AceCount;
	}
	paclNew = win_malloc(sizeof(ACL));
	paclNew->AclRevision = ACL_REVISION;
	paclNew->AclSize = sizeof(ACL);
	paclNew->AceCount = 0;
	while (wIndex--){
		GetAce(pAcl, wIndex, (PVOID)&pEntry);
		dwType = AclOwnerType(pEntry, &dwTypeMask);
		aceBuf.Header = pEntry->Header;
		aceBuf.Mask = pEntry->Mask;
		if (dwType == WIN_ACL_USER){
			if (!AclSidIsNone(NewUser)){
				aceBuf.Header.AceSize = aceSize + GetLengthSid(NewUser);
				CopySid(GetLengthSid(NewUser), &aceBuf.Sid, NewUser);
				pEntry = (ACE_PEEK *)&aceBuf;
			}
		}else if (dwType == WIN_ACL_GROUP){
			if (!AclSidIsNone(NewGroup)){
				aceBuf.Header.AceSize = aceSize + GetLengthSid(NewGroup);
				CopySid(GetLengthSid(NewGroup), &aceBuf.Sid, NewGroup);
				pEntry = (ACE_PEEK *)&aceBuf;
			}
		}
		paclNew->AclSize += pEntry->Header.AceSize;
		if (win_realloc(paclNew->AclSize, paclNew, (PVOID *)&paclNew)){
			AddAce(paclNew, ACL_REVISION, 0, pEntry, pEntry->Header.AceSize);
		}
	}
	if (!SetSecurityDescriptorDacl(Result, bPresent, paclNew, bDefaulted)){
		WIN_ERR("SetSecurityDescriptorDacl(): %s\n", win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	Control->Acl = paclNew;
	return(bResult);
}
BOOL 
vfs_acl_stat(PSECURITY_DESCRIPTOR Source, WIN_VATTR *Result)
{
	BOOL bResult = TRUE;
	ACE_PEEK *Info;
	WORD wIndex = 0;
	PACL acl = NULL;
	PSID pOwner, pGroup;
	BOOL bPresent, bDefaulted;
	DWORD dwType, dwTypeMask = 0;

	if (GetSecurityDescriptorOwner(Source, &pOwner, &bDefaulted)){
		CopySid(GetLengthSid(pOwner), &Result->UserSid, pOwner);
	}
	if (GetSecurityDescriptorGroup(Source, &pGroup, &bDefaulted)){
		CopySid(GetLengthSid(pGroup), &Result->GroupSid, pGroup);
	}
	if (!GetSecurityDescriptorDacl(Source, &bPresent, &acl, &bDefaulted)){
		WIN_ERR("GetSecurityDescriptorDacl(): %s\n", win_strerror(GetLastError()));
	}else if (bPresent && acl){
		wIndex = acl->AceCount;
	}else{		/* VFAT */
		Result->Mode.User = WIN_S_IRWX;
		Result->Mode.Group = WIN_S_IRWX;
		Result->Mode.Other = WIN_S_IRWX;
	}
	while (wIndex--){
		GetAce(acl, wIndex, (PVOID)&Info);
		dwType = AclOwnerType(Info, &dwTypeMask);
		if (dwType == WIN_ACL_USER){
			CopySid(GetLengthSid(&Info->Sid), &Result->UserSid, &Info->Sid);
			Result->Mode.User = Info->Mask;
		}else if (dwType == WIN_ACL_GROUP){
			CopySid(GetLengthSid(&Info->Sid), &Result->GroupSid, &Info->Sid);
			Result->Mode.Group = Info->Mask;
		}else if (dwType == WIN_ACL_OTHER){
			Result->Mode.Other = Info->Mask;
		}
	}
	if (EqualSid(pOwner, &SidAdmins)){
		Result->Mode.Special |= WIN_S_ISUID;
	}
	if (EqualSid(pGroup, &SidSystem)){
		Result->Mode.Special |= WIN_S_ISGID;
	}
	return(bResult);
}
