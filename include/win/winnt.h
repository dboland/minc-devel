/**
 * @file winnt.h
 * Copyright 2012, 2013 MinGW.org project
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/* 
 * IdentifierAuthority
 */

/* NT Authority (5) */

#define DOMAIN_GROUP_RID_USERS		513

/* Integrity (16) */

#define SECURITY_MANDATORY_SYSTEM_RID	0x4000
#define SECURITY_MANDATORY_HIGH_RID	0x3000
#define SECURITY_MANDATORY_MEDIUM_RID	0x2000
#define SECURITY_MANDATORY_LOW_RID	0x1000

/* 
 * SubAuthority
 */

#define SECURITY_NT_NON_UNIQUE		21
#define SECURITY_BUILTIN_DOMAIN		32

#define FILE_ATTRIBUTE_READONLY			0x00000001
#define FILE_ATTRIBUTE_HIDDEN			0x00000002
#define FILE_ATTRIBUTE_SYSTEM			0x00000004
#define FILE_ATTRIBUTE_DIRECTORY		0x00000010
#define FILE_ATTRIBUTE_ARCHIVE			0x00000020
#define FILE_ATTRIBUTE_DEVICE			0x00000040
#define FILE_ATTRIBUTE_NORMAL			0x00000080
#define FILE_ATTRIBUTE_TEMPORARY		0x00000100
#define FILE_ATTRIBUTE_REPARSE_POINT		0x00000400
#define FILE_ATTRIBUTE_COMPRESSED		0x00000800
#define FILE_ATTRIBUTE_OFFLINE			0x00001000
#define FILE_ATTRIBUTE_NOT_CONTENT_INDEXED	0x00002000
#define FILE_ATTRIBUTE_ENCRYPTED		0x00004000

/* Vista */

#define FILE_ATTRIBUTE_INTEGRITY_STREAM		0x00008000
#define FILE_ATTRIBUTE_VIRTUAL			0x00010000
#define FILE_ATTRIBUTE_VALID_FLAGS		0x00017fb7
#define FILE_ATTRIBUTE_VALID_SET_FLAGS		0x000031a7
#define FILE_ATTRIBUTE_EA			0x00040000
#define FILE_ATTRIBUTE_RECALL_ON_OPEN		0x00040000
#define FILE_ATTRIBUTE_PINNED			0x00080000
#define FILE_ATTRIBUTE_UNPINNED			0x00100000
#define FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS	0x00400000

#define FILE_LIST_DIRECTORY             0x00000001
#define FILE_READ_DATA                  0x00000001
#define FILE_ADD_FILE                   0x00000002
#define FILE_WRITE_DATA                 0x00000002
#define FILE_ADD_SUBDIRECTORY           0x00000004
#define FILE_APPEND_DATA                0x00000004
#define FILE_CREATE_PIPE_INSTANCE       0x00000004
#define FILE_READ_EA                    0x00000008
#define FILE_READ_PROPERTIES            0x00000008
#define FILE_WRITE_EA                   0x00000010
#define FILE_WRITE_PROPERTIES           0x00000010
#define FILE_EXECUTE                    0x00000020
#define FILE_TRAVERSE                   0x00000020
#define FILE_DELETE_CHILD               0x00000040
#define FILE_READ_ATTRIBUTES            0x00000080
#define FILE_WRITE_ATTRIBUTES           0x00000100

#define MAXIMUM_ALLOWED 0x02000000
#define GENERIC_READ    0x80000000
#define GENERIC_WRITE   0x40000000
#define GENERIC_EXECUTE 0x20000000
#define GENERIC_ALL     0x10000000

#define DELETE  0x00010000L
#define READ_CONTROL    0x020000L
#define WRITE_DAC       0x040000L
#define WRITE_OWNER     0x080000L
#define SYNCHRONIZE     0x100000L
#define STANDARD_RIGHTS_REQUIRED	0xF0000
#define STANDARD_RIGHTS_READ		0x20000
#define STANDARD_RIGHTS_WRITE		0x20000
#define STANDARD_RIGHTS_EXECUTE	0x20000
#define STANDARD_RIGHTS_ALL		0x1F0000
#define SPECIFIC_RIGHTS_ALL		0xFFFF

#define PROCESS_TERMINATE       1		/* OpenProcessToken() */
#define PROCESS_CREATE_THREAD   2		/* CheckTokenMembership() */
#define PROCESS_SET_SESSIONID   4
#define PROCESS_VM_OPERATION    8
#define PROCESS_VM_READ 16
#define PROCESS_VM_WRITE        32
#define PROCESS_DUP_HANDLE      64		/* DuplicateHandle() */
#define PROCESS_CREATE_PROCESS  128		/* CreateProcess() */
#define PROCESS_SET_QUOTA       256
#define PROCESS_SUSPEND_RESUME 0x0800
#define PROCESS_SET_INFORMATION 512
#define PROCESS_QUERY_INFORMATION       1024
#define PROCESS_QUERY_LIMITED_INFORMATION       0x1000
#define PROCESS_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0xFFF)

#define ANYSIZE_ARRAY 1

#define PAGE_NOACCESS   0x0001
#define PAGE_READONLY   0x0002
#define PAGE_READWRITE  0x0004
#define PAGE_WRITECOPY  0x0008
#define PAGE_EXECUTE    0x0010
#define PAGE_EXECUTE_READ       0x0020
#define PAGE_EXECUTE_READWRITE  0x0040
#define PAGE_EXECUTE_WRITECOPY  0x0080
#define PAGE_GUARD              0x0100
#define PAGE_NOCACHE            0x0200
#define PAGE_WRITECOMBINE 0x0400

#define FILE_SHARE_READ                 0x00000001
#define FILE_SHARE_WRITE                0x00000002
#define FILE_SHARE_DELETE               0x00000004

#define NTAPI __stdcall

#define MEM_COMMIT           0x1000
#define MEM_RESERVE          0x2000
#define MEM_DECOMMIT         0x4000
#define MEM_RELEASE          0x8000
#define MEM_FREE            0x10000
#define MEM_PRIVATE         0x20000
#define MEM_MAPPED          0x40000
#define MEM_RESET           0x80000
#define MEM_TOP_DOWN       0x100000
#define MEM_WRITE_WATCH    0x200000 /* 98/Me */
#define MEM_PHYSICAL       0x400000
#define MEM_4MB_PAGES    0x80000000

typedef char CHAR;
typedef unsigned char UCHAR;
typedef short SHORT;
typedef long LONG;
typedef unsigned long ULONG;
typedef __wchar_t WCHAR;
typedef __int64_t LONGLONG;
typedef __uint64_t DWORDLONG;
typedef void *PVOID, *LPVOID;
typedef unsigned long ULONG,*PULONG;

#include "basetsd.h"

#ifdef STRICT
typedef void *HANDLE;
#define DECLARE_HANDLE(n) typedef struct n##__{int i;}*n
#else
typedef PVOID HANDLE;
#define DECLARE_HANDLE(n) typedef HANDLE n
#endif

typedef PVOID PSID;
typedef HANDLE HINSTANCE;
typedef HINSTANCE HMODULE;
typedef LONG HRESULT;
typedef CHAR *LPSTR;
typedef CONST CHAR *LPCSTR;
typedef WCHAR *LPWSTR;
typedef CONST WCHAR *LPCWSTR;
typedef HANDLE HLOCAL;
typedef HANDLE *PHANDLE;
typedef BOOL *LPBOOL;

DECLARE_HANDLE(HKEY);

/* GetAdaptersAddresses() */

typedef DWORDLONG ULONGLONG,*PULONGLONG;
typedef WCHAR *PWCHAR,*LPWCH,*PWCH,*NWPSTR,*LPWSTR,*PWSTR;
typedef CHAR *PCHAR,*LPCH,*PCH,*NPSTR,*LPSTR,*PSTR;

typedef struct _SID_IDENTIFIER_AUTHORITY {
        BYTE Value[6];
} SID_IDENTIFIER_AUTHORITY;

typedef struct _ACE_HEADER {
        BYTE AceType;
        BYTE AceFlags;
        WORD AceSize;
} ACE_HEADER;

typedef DWORD ACCESS_MASK;

typedef struct _ACL {
        BYTE AclRevision;
        BYTE Sbz1;
        WORD AclSize;
        WORD AceCount;
        WORD Sbz2;
} ACL,*PACL;

typedef WORD SECURITY_DESCRIPTOR_CONTROL,*PSECURITY_DESCRIPTOR_CONTROL;
typedef struct _SECURITY_DESCRIPTOR {
        BYTE Revision;
        BYTE Sbz1;
        SECURITY_DESCRIPTOR_CONTROL Control;
        PSID Owner;
        PSID Group;
        PACL Sacl;
        PACL Dacl;
} SECURITY_DESCRIPTOR, *PSECURITY_DESCRIPTOR, *PISECURITY_DESCRIPTOR;

typedef PVOID PSID;

typedef struct _SID {
   BYTE  Revision;
   BYTE  SubAuthorityCount;
   SID_IDENTIFIER_AUTHORITY IdentifierAuthority;
   DWORD SubAuthority[ANYSIZE_ARRAY];
} SID, *PISID;

typedef struct _FLOATING_SAVE_AREA {
     ULONG ControlWord;
     ULONG StatusWord;
     ULONG TagWord;
     ULONG ErrorOffset;
     ULONG ErrorSelector;
     ULONG DataOffset;
     ULONG DataSelector;
     UCHAR RegisterArea[80];
     ULONG Cr0NpxState;
} FLOATING_SAVE_AREA, *PFLOATING_SAVE_AREA;

typedef struct _CONTEXT {
     ULONG ContextFlags;
     ULONG Dr0;
     ULONG Dr1;
     ULONG Dr2;
     ULONG Dr3;
     ULONG Dr6;
     ULONG Dr7;
     FLOATING_SAVE_AREA FloatSave;
     ULONG SegGs;
     ULONG SegFs;
     ULONG SegEs;
     ULONG SegDs;
     ULONG Edi;
     ULONG Esi;
     ULONG Ebx;
     ULONG Edx;
     ULONG Ecx;
     ULONG Eax;
     ULONG Ebp;
     ULONG Eip;
     ULONG SegCs;
     ULONG EFlags;
     ULONG Esp;
     ULONG SegSs;
     UCHAR ExtendedRegisters[512];
} CONTEXT, *PCONTEXT, *LPCONTEXT;

typedef unsigned short USHORT,*PUSHORT;

typedef struct _GUID {
        unsigned long  Data1;
        unsigned short Data2;
        unsigned short Data3;
        unsigned char  Data4[8];
} GUID, *REFGUID, *LPGUID;

typedef struct _MEMORY_BASIC_INFORMATION {
        PVOID BaseAddress;
        PVOID AllocationBase;
        DWORD AllocationProtect;
        DWORD RegionSize;
        DWORD State;
        DWORD Protect;
        DWORD Type;
} MEMORY_BASIC_INFORMATION,*PMEMORY_BASIC_INFORMATION;

/* 
 * ntapi.h
 */

typedef enum tagTOKEN_TYPE {
        TokenPrimary = 1,
        TokenImpersonation
} TOKEN_TYPE,*PTOKEN_TYPE;
typedef struct _LUID {
	DWORD LowPart;
	LONG HighPart;
} LUID, *PLUID;
typedef struct _SID_AND_ATTRIBUTES {
    PSID  Sid; 
    DWORD Attributes; 
} SID_AND_ATTRIBUTES; 
typedef struct _TOKEN_GROUPS {
	DWORD GroupCount; 
	SID_AND_ATTRIBUTES Groups[ANYSIZE_ARRAY]; 
} TOKEN_GROUPS,*PTOKEN_GROUPS;
typedef struct _LUID_AND_ATTRIBUTES {
    LUID  Luid; 
    DWORD Attributes; 
} LUID_AND_ATTRIBUTES; 
typedef struct _TOKEN_PRIVILEGES {
    DWORD PrivilegeCount; 
    LUID_AND_ATTRIBUTES Privileges[ANYSIZE_ARRAY]; 
} TOKEN_PRIVILEGES,*PTOKEN_PRIVILEGES; 
typedef struct _TOKEN_DEFAULT_DACL {
    PACL DefaultDacl; 
} TOKEN_DEFAULT_DACL,*PTOKEN_DEFAULT_DACL;

/* 
 * ntdef.h
 */

typedef LONG NTSTATUS, *PNTSTATUS;

/* 
 * ddk/ntifs.h
 */

#define OB_TYPE_TYPE		1
#define OB_TYPE_DIRECTORY		2
#define OB_TYPE_SYMBOLIC_LINK	3
#define OB_TYPE_TOKEN		4
#define OB_TYPE_PROCESS		5
#define OB_TYPE_THREAD		6
#define OB_TYPE_EVENT		7
#define OB_TYPE_EVENT_PAIR		8
#define OB_TYPE_MUTANT		9
#define OB_TYPE_SEMAPHORE		10
#define OB_TYPE_TIMER		11
#define OB_TYPE_PROFILE		12
#define OB_TYPE_WINDOW_STATION	13
#define OB_TYPE_DESKTOP		14
#define OB_TYPE_SECTION		15
#define OB_TYPE_KEY			16
#define OB_TYPE_PORT		17
#define OB_TYPE_ADAPTER		18
#define OB_TYPE_CONTROLLER		19
#define OB_TYPE_DEVICE		20
#define OB_TYPE_DRIVER	       21
#define OB_TYPE_IO_COMPLETION	22
#define OB_TYPE_FILE	       23

#define FILE_CASE_SENSITIVE_SEARCH      0x00000001
#define FILE_CASE_PRESERVED_NAMES       0x00000002
#define FILE_UNICODE_ON_DISK            0x00000004
#define FILE_PERSISTENT_ACLS            0x00000008
#define FILE_FILE_COMPRESSION           0x00000010
#define FILE_VOLUME_QUOTAS              0x00000020
#define FILE_SUPPORTS_SPARSE_FILES      0x00000040
#define FILE_SUPPORTS_REPARSE_POINTS    0x00000080
#define FILE_SUPPORTS_REMOTE_STORAGE    0x00000100
#define FILE_RETURNS_CLEANUP_RESULT_INFO 0x00000200
#define FS_LFN_APIS                     0x00004000
#define FILE_VOLUME_IS_COMPRESSED       0x00008000
#define FILE_SUPPORTS_OBJECT_IDS        0x00010000
#define FILE_SUPPORTS_ENCRYPTION        0x00020000
#define FILE_NAMED_STREAMS              0x00040000
#define FILE_READ_ONLY_VOLUME           0x00080000
#define FILE_SEQUENTIAL_WRITE_ONCE      0x00100000
#define FILE_SUPPORTS_TRANSACTIONS      0x00200000
#define FILE_SUPPORTS_HARD_LINKS        0x00400000
#define FILE_SUPPORTS_EXTENDED_ATTRIBUTES 0x00800000
#define FILE_SUPPORTS_OPEN_BY_FILE_ID   0x01000000
#define FILE_SUPPORTS_USN_JOURNAL       0x02000000

