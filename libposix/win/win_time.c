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

/************************************************************/

VOID 
TimeInit(LARGE_INTEGER *Frequency)
{
	if (!QueryPerformanceFrequency(Frequency)){
		WIN_ERR("QueryPerformanceFrequency(): %s\n", win_strerror(GetLastError()));
	}
}

/************************************************************/

BOOL 
win_clock_gettime_MONOTONIC(DWORDLONG *Result)
{
	BOOL bResult = FALSE;
	LARGE_INTEGER liCount;
	DWORD dwFrequency;

	if (!QueryPerformanceCounter(&liCount)){
		WIN_ERR("QueryPerformanceCounter(): %s\n", win_strerror(GetLastError()));
	}else{
		dwFrequency = 1000000000LL / __Frequency->QuadPart;
		*Result = liCount.QuadPart * dwFrequency;
		bResult = TRUE;
	}
	return(bResult);
}

/************************************************************/

BOOL 
win_settimeofday(DWORDLONG Time, WORD MilliSeconds)
{
	BOOL bResult = FALSE;
	struct tm *ptm = NULL;
	SYSTEMTIME sTime = {0};

	if (ptm = _localtime64(&Time)){
		sTime.wYear = ptm->tm_year + 1900;
		sTime.wMonth = ptm->tm_mon + 1;
		sTime.wDayOfWeek = ptm->tm_wday;
		sTime.wDay = ptm->tm_mday;
		sTime.wHour = ptm->tm_hour;
		sTime.wMinute = ptm->tm_min;
		sTime.wSecond = ptm->tm_sec;
		sTime.wMilliseconds = MilliSeconds;
		bResult = SetLocalTime(&sTime);
	}else{
		WIN_ERR("_localtime64(%I64d): %s\n", Time, win_strerror(GetLastError()));
	}
	return(bResult);
}
