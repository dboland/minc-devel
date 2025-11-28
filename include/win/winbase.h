/**
 * @file winbase.h
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

#define WINBASEAPI DECLSPEC_IMPORT
#define WINADVAPI DECLSPEC_IMPORT

#define CREATE_NEW      1
#define CREATE_ALWAYS   2
#define OPEN_EXISTING   3
#define OPEN_ALWAYS     4
#define TRUNCATE_EXISTING      5

#define STD_INPUT_HANDLE (DWORD)(0xfffffff6)
#define STD_OUTPUT_HANDLE (DWORD)(0xfffffff5)
#define STD_ERROR_HANDLE (DWORD)(0xfffffff4)
#define INVALID_HANDLE_VALUE (HANDLE)(-1)

#define INFINITE        0xFFFFFFFF

#define HANDLE_FLAG_INHERIT             0x01
#define HANDLE_FLAG_PROTECT_FROM_CLOSE  0x02

#define FILE_BEGIN      0
#define FILE_CURRENT    1
#define FILE_END        2

#define FILE_FLAG_BACKUP_SEMANTICS	0x02000000
#define FILE_FLAG_OVERLAPPED		0x40000000

#define MAXIMUM_WAIT_OBJECTS 64

#define LOCKFILE_EXCLUSIVE_LOCK	0x00000002
#define LOCKFILE_FAIL_IMMEDIATELY	0x00000001

#define PIPE_TYPE_BYTE			0
#define PIPE_TYPE_MESSAGE		4
#define PIPE_READMODE_BYTE		0
#define PIPE_READMODE_MESSAGE		2

/* #define WAIT_OBJECT_0 0
#define WAIT_ABANDONED_0 128
#define WAIT_TIMEOUT 258
#define WAIT_IO_COMPLETION 0xC0
#define WAIT_ABANDONED 128
#define WAIT_FAILED ((DWORD)0xFFFFFFFF)
*/
#define __AW__(AW, AW_) AW ## AW_
#define __AW(AW) __AW__(AW, A)

#define DDD_RAW_TARGET_PATH		0x00000001
#define DDD_REMOVE_DEFINITION		0x00000002
#define DDD_EXACT_MATCH_ON_REMOVE	0x00000004
#define DDD_NO_BROADCAST_SYSTEM		0x00000008

typedef struct _FILETIME {
        DWORD dwLowDateTime;
        DWORD dwHighDateTime;
} FILETIME, *LPFILETIME;

typedef struct _SYSTEM_INFO {
        _ANONYMOUS_UNION union {
                DWORD dwOemId;
                _ANONYMOUS_STRUCT struct {
                        WORD wProcessorArchitecture;
                        WORD wReserved;
                } DUMMYSTRUCTNAME;
        } DUMMYUNIONNAME;
        DWORD dwPageSize;
        PVOID lpMinimumApplicationAddress;
        PVOID lpMaximumApplicationAddress;
        DWORD dwActiveProcessorMask;
        DWORD dwNumberOfProcessors;
        DWORD dwProcessorType;
        DWORD dwAllocationGranularity;
        WORD wProcessorLevel;
        WORD wProcessorRevision;
} SYSTEM_INFO,*LPSYSTEM_INFO;

typedef struct _MEMORYSTATUS {
        DWORD dwLength;
        DWORD dwMemoryLoad;
        DWORD dwTotalPhys;
        DWORD dwAvailPhys;
        DWORD dwTotalPageFile;
        DWORD dwAvailPageFile;
        DWORD dwTotalVirtual;
        DWORD dwAvailVirtual;
} MEMORYSTATUS,*LPMEMORYSTATUS;

typedef struct _MEMORYSTATUSEX {
        DWORD dwLength;
        DWORD dwMemoryLoad;
        DWORDLONG ullTotalPhys;
        DWORDLONG ullAvailPhys;
        DWORDLONG ullTotalPageFile;
        DWORDLONG ullAvailPageFile;
        DWORDLONG ullTotalVirtual;
        DWORDLONG ullAvailVirtual;
        DWORDLONG ullAvailExtendedVirtual;
} MEMORYSTATUSEX,*LPMEMORYSTATUSEX;

typedef struct _SYSTEMTIME {
        WORD wYear;
        WORD wMonth;
        WORD wDayOfWeek;
        WORD wDay;
        WORD wHour;
        WORD wMinute;
        WORD wSecond;
        WORD wMilliseconds;
} SYSTEMTIME,*LPSYSTEMTIME;

typedef union _LARGE_INTEGER {
	struct {
		DWORD LowPart;
		LONG HighPart;
	};
	ULONGLONG QuadPart;
} LARGE_INTEGER,*LPLARGE_INTEGER;

typedef DWORD (WINAPI *LPTHREAD_START_ROUTINE)(LPVOID);

typedef struct _STARTUPINFOA {
        DWORD   cb;
        LPSTR   lpReserved;
        LPSTR   lpDesktop;
        LPSTR   lpTitle;
        DWORD   dwX;
        DWORD   dwY;
        DWORD   dwXSize;
        DWORD   dwYSize;
        DWORD   dwXCountChars;
        DWORD   dwYCountChars;
        DWORD   dwFillAttribute;
        DWORD   dwFlags;
        WORD    wShowWindow;
        WORD    cbReserved2;
        PBYTE   lpReserved2;
        HANDLE  hStdInput;
        HANDLE  hStdOutput;
        HANDLE  hStdError;
} STARTUPINFOA,*LPSTARTUPINFOA;

typedef struct _STARTUPINFOW {
        DWORD   cb;
        LPWSTR  lpReserved;
        LPWSTR  lpDesktop;
        LPWSTR  lpTitle;
        DWORD   dwX;
        DWORD   dwY;
        DWORD   dwXSize;
        DWORD   dwYSize;
        DWORD   dwXCountChars;
        DWORD   dwYCountChars;
        DWORD   dwFillAttribute;
        DWORD   dwFlags;
        WORD    wShowWindow;
        WORD    cbReserved2;
        PBYTE   lpReserved2;
        HANDLE  hStdInput;
        HANDLE  hStdOutput;
        HANDLE  hStdError;
} STARTUPINFOW,*LPSTARTUPINFOW;

typedef __AW(STARTUPINFO) STARTUPINFO,*LPSTARTUPINFO;

/* basetyps.h */

typedef GUID CLSID;

WINBASEAPI DWORD WINAPI GetLastError(VOID);
WINBASEAPI VOID WINAPI SetLastError(DWORD);
WINBASEAPI VOID WINAPI GetSystemTimeAsFileTime(LPFILETIME);
WINBASEAPI BOOL WINAPI TlsSetValue(DWORD,PVOID);
WINBASEAPI PVOID WINAPI TlsGetValue(DWORD);
WINBASEAPI BOOL WINAPI EqualPrefixSid(PSID,PSID);
WINBASEAPI VOID WINAPI GetSystemInfo(LPSYSTEM_INFO);
WINBASEAPI BOOL WINAPI GlobalMemoryStatusEx(LPMEMORYSTATUSEX);
WINBASEAPI VOID WINAPI GetSystemTime(LPSYSTEMTIME);
WINBASEAPI BOOL WINAPI SetSystemTime(const SYSTEMTIME*);
WINBASEAPI BOOL WINAPI SetEvent(HANDLE);
WINBASEAPI BOOL WINAPI GetSystemTimes(LPFILETIME,LPFILETIME,LPFILETIME);
WINBASEAPI BOOL WINAPI CopySid(DWORD,PSID,PSID);
WINBASEAPI DWORD WINAPI WaitForSingleObjectEx(HANDLE,DWORD,BOOL);
WINBASEAPI DWORD WINAPI SuspendThread(HANDLE);
