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

#include <ddk/winddk.h>

/************************************************************/

WIN_TASK *
ProcCreateTask(DWORD Offset)
{
	WIN_TASK *pwTask = &__Tasks[Offset];
	DWORD dwAffinity = 0;

	while (Offset < WIN_CHILD_MAX){
		if (!pwTask->Flags){
			pwTask->Flags = WIN_PS_EMBRYO;
			pwTask->TaskId = Offset;
			pwTask->ThreadId = GetCurrentThreadId();
			pwTask->ProcessId = GetCurrentProcessId();
			pwTask->Processor = KeGetCurrentProcessorNumber();
			win_clock_gettime_MONOTONIC(&pwTask->ClockTime);
			GetSystemTimeAsFileTime(&pwTask->Started);
			return(pwTask);
		}else if (pwTask->Flags & WIN_PS_NOZOMBIE){
			ZeroMemory(pwTask, sizeof(WIN_TASK));
		}
		Offset++;
		pwTask++;
	}
	WIN_ERR("ProcCreateTask(%d): %s\n", Offset, win_strerror(ERROR_MAX_THRDS_REACHED));
	vfs_raise(WM_SIGNAL, CTRL_BREAK_EVENT, 0);
	return(NULL);
}
