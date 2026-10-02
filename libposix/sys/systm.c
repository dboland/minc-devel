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

#include "../kern/systm.h"

/****************************************************/

void 
cpu_configure(void)
{
	WIN_CFDATA cfData;
	WIN_CFDRIVER cfDriver;
	WIN_STATVFS fsInfo;
	DWORD dwFlags = WIN_MNT_NOWAIT;
	CHAR szMessage[MAX_MESSAGE];
	LONG lResult;

	/* Determine i/o configuration for a machine.
	 */
	if (!vfs_setconf(&cfData, dwFlags)){
		return;
	}else while (vfs_getconf(&cfData, dwFlags)){
		if (cfData.FSType == FS_TYPE_DRIVE){
			drive_lookup(&cfData, dwFlags, &fsInfo);
			if (!drive_match(&cfData, &fsInfo)){
				msgbuf_DRIVE(&cfData, &fsInfo, szMessage);
				WIN_ERR(szMessage);
			}
		}
	}
	vfs_endconf(&cfData);
}
void 
diskconf(char *root)
{
	/* Configure root/swap devices
	 */
	win_chroot(__Globals->Root);
	win_wcstombs(root, __Globals->Root, MAX_PATH);
}
void 
consinit(void)
{
	/* sys/arch/i386/i386/machdep.c
	*/
	char_init();
}
