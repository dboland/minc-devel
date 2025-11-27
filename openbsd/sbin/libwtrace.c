#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <aclapi.h>
#include <accctrl.h>

#include <ddk/ntapi.h>
#include <ddk/ntifs.h>	// Object types

#include <stdio.h>
#include <stdlib.h>

#include "win_types.h"
#include "dev_types.h"
#include "vfs_types.h"
#include "ntdll_posix.h"

#include "../libtrace/libtrace.h"

LPSTR win_strsid(PSID Sid);
LPSTR win_strerror(HRESULT Error);

#define WIN_ERR		printf

extern __attribute__((dllimport)) WIN_MOUNT	*__Mounts;

/************************************************************/

HANDLE 
GetProcessHandle(DWORD ProcessId)
{
	HANDLE hResult = NULL;

	if (!ProcessId){
		hResult = GetCurrentProcess();
	}else if (!(hResult = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessId))){
		WIN_ERR("OpenProcess(%d): %s\n", ProcessId, win_strerror(GetLastError()));
	}
	return(hResult);
}
HANDLE 
GetThreadHandle(DWORD ThreadId)
{
	HANDLE hResult = NULL;

	if (!ThreadId){
		hResult = GetCurrentThread();
	}else if (!(hResult = OpenThread(THREAD_ALL_ACCESS, FALSE, ThreadId))){
		WIN_ERR("OpenThread(%d): %s\n", ThreadId, win_strerror(GetLastError()));
	}
	return(hResult);
}

/************************************************************/

VOID 
wtrace_ACL_DESKTOP(LPSTR Buffer)
{
	PACL Acl = NULL;
	PSECURITY_DESCRIPTOR Sd;
	HDESK hObject = OpenInputDesktop(0, FALSE, READ_CONTROL | DESKTOP_READOBJECTS);

	GetSecurityInfo(hObject, SE_WINDOW_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, &Acl, NULL, &Sd);
	if (Acl){
		win_ACL(Buffer, "ACL Desktop", Acl, OB_TYPE_DESKTOP);
		LocalFree(Sd);
	}else{
		WIN_ERR("GetSecurityInfo(%d): %s\n", hObject, win_strerror(GetLastError()));
	}
	CloseDesktop(hObject);
}
VOID 
wtrace_ACL_STATION(LPSTR Buffer)
{
	PACL Acl = NULL;
	PSECURITY_DESCRIPTOR Sd;
	HWINSTA hObject = GetProcessWindowStation();

	GetSecurityInfo(hObject, SE_WINDOW_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, &Acl, NULL, &Sd);
	if (Acl){
		win_ACL(Buffer, "ACL WindowStation", Acl, OB_TYPE_WINDOW_STATION);
		LocalFree(Sd);
	}else{
		WIN_ERR("GetSecurityInfo(%d): %s\n", hObject, win_strerror(GetLastError()));
	}
}
VOID 
wtrace_ACL_PROCESS(LPSTR Buffer)
{
	DWORD dwType = OWNER_SECURITY_INFORMATION + GROUP_SECURITY_INFORMATION + DACL_SECURITY_INFORMATION;
	PACL Acl = NULL;
	PSECURITY_DESCRIPTOR psd;
	DWORD dwSize = 0;
	HANDLE hProcess = GetCurrentProcess();

	GetUserObjectSecurity(hProcess, &dwType, NULL, dwSize, &dwSize);
	psd = LocalAlloc(LMEM_FIXED, dwSize);
	if (!GetUserObjectSecurity(hProcess, &dwType, psd, dwSize, &dwSize)){
		WIN_ERR("GetUserObjectSecurity(%d): %s\n", hProcess, win_strerror(GetLastError()));
	}else{
		win_SECURITY_DESCRIPTOR(psd, OB_TYPE_PROCESS, Buffer);
	}
	LocalFree(psd);
}
VOID 
wtrace_ACL_FILE(LPCWSTR FileName, LPSTR Buffer)
{
	DWORD dwType = OWNER_SECURITY_INFORMATION + GROUP_SECURITY_INFORMATION + DACL_SECURITY_INFORMATION;
	PSECURITY_DESCRIPTOR psd;
	DWORD dwSize = 0;
	BOOL bDefaulted;
	BOOL bPresent;
	PACL Acl;
	WCHAR szPath[MAX_PATH];
	LPWSTR pszBase = NULL;

	GetFileSecurityW(FileName, dwType, NULL, dwSize, &dwSize);
	psd = LocalAlloc(LMEM_FIXED, dwSize);
	if (!GetFileSecurityW(FileName, dwType, psd, dwSize, &dwSize)){
		WIN_ERR("GetFileSecurity(%ls): %s\n", FileName, win_strerror(GetLastError()));
	}else if (!GetFullPathNameW(FileName, MAX_PATH, szPath, &pszBase)){
		WIN_ERR("GetFullPathName(%ls): %s", FileName, win_strerror(GetLastError()));
	}else{
		printf("FileName: %ls\n", szPath);
		win_SECURITY_DESCRIPTOR(psd, OB_TYPE_FILE, Buffer);
	}
	LocalFree(psd);
}
VOID 
wtrace_ACL_OBJECT(LPCSTR Name, LPSTR Buffer)
{
	DWORD dwType = OWNER_SECURITY_INFORMATION + GROUP_SECURITY_INFORMATION + DACL_SECURITY_INFORMATION;
	PSECURITY_DESCRIPTOR psd;
	DWORD dwSize = 0;
	BOOL bDefaulted;
	BOOL bPresent;
	PACL Acl;

	GetFileSecurity(Name, dwType, NULL, dwSize, &dwSize);
	psd = LocalAlloc(LMEM_FIXED, dwSize);
	if (!GetFileSecurity(Name, dwType, psd, dwSize, &dwSize)){
		WIN_ERR("GetFileSecurity(%s): %s\n", Name, win_strerror(GetLastError()));
	}else{
		printf("Resolved: %s\n", Name);
		win_SECURITY_DESCRIPTOR(psd, OB_TYPE_PROCESS, Buffer);
	}
	LocalFree(psd);
}
VOID 
wtrace_TOKEN_PROCESS(DWORD ProcessId, LPSTR Buffer)
{
	HANDLE hToken;

	if (!OpenProcessToken(GetProcessHandle(ProcessId), MAXIMUM_ALLOWED, &hToken)){
		WIN_ERR("GetCurrentProcess(%d): %s\n", ProcessId, win_strerror(GetLastError()));
	}else{
		win_TOKEN(hToken, Buffer);
		CloseHandle(hToken);
	}
}
VOID 
wtrace_TOKEN_THREAD(DWORD ThreadId, LPSTR Buffer)
{
	HANDLE hToken = NULL;

	if (!OpenThreadToken(GetThreadHandle(ThreadId), MAXIMUM_ALLOWED, TRUE, &hToken)){
		WIN_ERR("OpenThreadToken(%d): %s\n", ThreadId, win_strerror(GetLastError()));
	}else{
		win_TOKEN(hToken, Buffer);
		CloseHandle(hToken);
	}
}
VOID 
wtrace_SID_RIGHTS(LPCSTR Name, LPSTR Buffer)
{
	LPSTR psz = Buffer;
	SID_NAME_USE snuType = 0;
	SID8 sid;
	DWORD sidSize = sizeof(SID8);
	CHAR szDomain[MAX_NAME];
	DWORD domSize = MAX_NAME;

	if (!LookupAccountName(NULL, Name, &sid, &sidSize, szDomain, &domSize, &snuType)){
		WIN_ERR("LookupAccountName(%s): %s\n", Name, win_strerror(GetLastError()));
	}else{
		psz += sprintf(psz, "%s (%s: %s\\%s)\n", win_strsid(&sid), __STYPE[snuType], szDomain, Name);
		win_SID_RIGHTS(psz, "Capabilities", &sid);
	}
}
VOID 
wtrace_SYSTEM_INFO(LPSTR Buffer)
{
	SYSTEM_INFO sInfo;

	GetSystemInfo(&sInfo);
	win_SYSTEM_INFO(Buffer, "GetSystemInfo", &sInfo);
}
VOID 
wtrace_MOUNT(CHAR Char, LPSTR Buffer)
{
	DWORD dwMountId = MOUNTID(Char);
	
	if (!vfs_MOUNT(&__Mounts[dwMountId], Buffer)){
		WIN_ERR("wtrace_MOUNT(%c): %s\n", Char, win_strerror(GetLastError()));
	}
}
VOID 
wtrace_NAMEI(WIN_NAMEIDATA *Path, LPSTR Buffer)
{
	Buffer += sprintf(Buffer, "wtrace_NAMEI");
	if (!vfs_NAMEI(Path, Buffer)){
		WIN_ERR("wtrace_NAMEI(%ls): %s\n", Path->Resolved, win_strerror(GetLastError()));
	}
}

/************************************************************/

VOID 
win_ktrace(STRUCT_TYPE Type, LONG Size, PVOID Data)
{
	LPSTR pszBuffer = LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, Size);

	switch (Type){
		case STRUCT_TOKEN_PROCESS:
			wtrace_TOKEN_PROCESS(atoi(Data), pszBuffer);
			break;
		case STRUCT_TOKEN_THREAD:
			wtrace_TOKEN_THREAD(atoi(Data), pszBuffer);
			break;
		case STRUCT_ACL_DESKTOP:
			wtrace_ACL_DESKTOP(pszBuffer);
			break;
		case STRUCT_ACL_STATION:
			wtrace_ACL_STATION(pszBuffer);
			break;
		case STRUCT_ACL_OBJECT:
			wtrace_ACL_OBJECT((LPCSTR)Data, pszBuffer);
			break;
		case STRUCT_ACL_FILE:
			wtrace_ACL_FILE((LPCWSTR)Data, pszBuffer);
			break;
		case STRUCT_ACL_PROCESS:
			wtrace_ACL_PROCESS(pszBuffer);
			break;
		case STRUCT_SID_RIGHTS:
			wtrace_SID_RIGHTS((LPCSTR)Data, pszBuffer);
			break;
		case STRUCT_SYSTEM_INFO:
			wtrace_SYSTEM_INFO(pszBuffer);
			break;
		case STRUCT_MOUNT:
			wtrace_MOUNT(*(CHAR *)Data, pszBuffer);
			break;
		case STRUCT_NAMEI:
			wtrace_NAMEI((WIN_NAMEIDATA *)Data, pszBuffer);
			break;
	}
	printf(pszBuffer);
	LocalFree(pszBuffer);
}
