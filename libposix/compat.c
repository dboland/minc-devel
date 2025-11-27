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

/************************************************************/

HANDLE 
CompatDevice(DWORD Index, WIN_DEVICE *Device)
{
	if (!Index){
		return(Device->Input);
	}else{
		return(Device->Output);
	}
}
HANDLE 
CompatHandle(DWORD Index, WIN_VNODE Nodes[])
{
	HANDLE hResult;
	WIN_VNODE *pwNode = &Nodes[Index];

	switch (pwNode->FSType){
		case FS_TYPE_PDO:
			hResult = CompatDevice(Index, DEVICE(pwNode->DeviceId));
			break;
		default:
			hResult = win_F_SETFD(pwNode->Handle, HANDLE_FLAG_INHERIT);
	}
	return(hResult);
}

/************************************************************/

int 
runcmd(char *argv[])
{
	int result = 0;
	CHAR szPath[PATH_MAX];
	WIN_TASK *pwTask = &__Tasks[CURRENT];
	WIN_VNODE *Nodes = pwTask->Node;
	STARTUPINFO si = {0};

	si.hStdInput = CompatHandle(0, Nodes);
	si.hStdOutput = CompatHandle(1, Nodes);
	si.hStdError = CompatHandle(2, Nodes);
	win_wcstombs(szPath, __Strings[pwTask->TaskId].Path, PATH_MAX);
	if (!win_execve(argv_win(pwTask, *argv, argv), szPath, &si)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
