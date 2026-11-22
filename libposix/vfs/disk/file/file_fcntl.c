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

/****************************************************/

BOOL 
file_F_SETLK(WIN_VNODE *Node, DWORD Flags, LARGE_INTEGER *Offset, LARGE_INTEGER *Size)
{
	OVERLAPPED ovl = {0, 0, Offset->LowPart, Offset->HighPart, Node->Event};
	DWORDLONG dwlSegment = Size->QuadPart * Offset->QuadPart;

	/* Windows locking works opposite to POSIX locking. Exclusive locks
	 * cannot overlap an existing locked region of a file. Shared locks
	 * can overlap a locked region provided locks held on that region
	 * are shared locks.
	 */
	if (UnlockFileEx(Node->Handle, 0, Size->LowPart, Size->HighPart, &ovl)){
		Node->LockRegion -= dwlSegment;
		Node->LockSize -= Size->QuadPart;
	}else if (ERROR_NOT_LOCKED != GetLastError()){
		return(FALSE);
	}
	if (Flags == LOCKFILE_UNLOCK){
		return(TRUE);
	}else if (!LockFileEx(Node->Handle, Flags, 0, Size->LowPart, Size->HighPart, &ovl)){
		return(FALSE);
	}else{
		Node->LockRegion += dwlSegment;
		Node->LockSize += Size->QuadPart;
	}
	return(TRUE);
}
