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

DWORD 
WaitGetObjects(WIN_TASK *Children[], HANDLE Result[])
{
	DWORD dwResult = 0;
	WIN_TASK *pwTask = NULL;

	Result[dwResult++] = __Interrupt;
	while (pwTask = *Children++){
		Result[dwResult++] = pwTask->Handle;
	}
	return(dwResult);
}
BOOL 
WaitNoHang(WIN_TASK *Children[], DWORD Status, WIN_RUSAGE *Result)
{
	WIN_TASK *pwTask = NULL;

	while (pwTask = *Children++){
		if (pwTask->Flags & WIN_PS_NOZOMBIE){   /* recursive wait (NSIS Installer) */
			ZeroMemory(pwTask, sizeof(WIN_TASK));
		}else if (pwTask->Flags & WIN_PS_ZOMBIE){
			Result->TaskId = pwTask->TaskId;
			Result->Status = pwTask->Status;
			Result->UserTime += pwTask->UserTime;
			Result->KernelTime += pwTask->KernelTime;
			proc_close(pwTask);
			return(TRUE);
		}else if (pwTask->Status & Status){
			Result->TaskId = pwTask->TaskId;
			Result->Status = pwTask->Status;
			Result->UserTime += pwTask->UserTime;
			Result->KernelTime += pwTask->KernelTime;
			return(TRUE);
		}
	}
	SetLastError(ERROR_INVALID_THREAD_ID);
	return(FALSE);
}
BOOL 
WaitTimeOut(WIN_TASK *Task, WIN_TASK *Children[], DWORD TimeOut, WIN_RUSAGE *Result)
{
	BOOL bResult = FALSE;
	HANDLE hObjects[MAXIMUM_WAIT_OBJECTS];
	DWORD dwCount = WaitGetObjects(Children, hObjects);

	if (WAIT_FAILED == WaitForMultipleObjectsEx(dwCount, hObjects, FALSE, TimeOut, TRUE)){
		WIN_ERR("WaitForMultipleObjectsEx(%s): %s\n", win_strobj(hObjects, 2), win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}

/************************************************************/

BOOL 
vfs_wait4(WIN_TASK *Task, WIN_TASK *Children[], BOOL NoHang, DWORD Status, WIN_RUSAGE *Result)
{
	BOOL bResult = FALSE;

	Task->State = WIN_SSLEEP;
	while (!bResult){
		if (WaitNoHang(Children, Status, Result)){
			bResult = TRUE;
		}else if (NoHang){
			bResult = TRUE;
		}else if (!WaitTimeOut(Task, Children, INFINITE, Result)){
			break;
		}else if (proc_poll(Task)){
			break;
		}
	}
	Task->State = WIN_SONPROC;
	return(bResult);
}
