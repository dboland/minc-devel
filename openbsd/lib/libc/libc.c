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

#include <stdlib.h>
#include <stdio.h>

#include "win/windows.h"
#include "win_types.h"
#include "vfs_posix.h"
#include "msvc_posix.h"
#include "bsd_posix.h"

/* winnt.h */

typedef struct _IMAGE_TLS_DIRECTORY {
        PVOID StartAddressOfRawData;
        PVOID EndAddressOfRawData;
        PVOID AddressOfIndex;
        PVOID AddressOfCallBacks;
        PVOID SizeOfZeroFill;
        PVOID Characteristics;
} IMAGE_TLS_DIRECTORY,*PIMAGE_TLS_DIRECTORY;

#include <libgen.h>
#include <nlist.h>
#include <string.h>

extern __import CHAR 	*__PROGNAME;

#define GOT_MAX		32

typedef struct {
	void *address;
	size_t size;
} tlsentry;

/* In GCC, 'environ' seems to be a reserved word. Without 'dllexport' 
 * and the --export-all flag in LD, it will be omitted in the import 
 * table.
 */
char *__progname;
char **environ __attribute__((dllexport));
int h_errno;

tlsentry __offsets[GOT_MAX];

void 
_init(char *arg0, exec_t *exec)
{
	msvc_init(&exec->argc, &exec->argv, &exec->env);
	task_init(arg0, exec);
	environ = exec->env;
	__progname = __PROGNAME = basename(arg0);
}
int 
__fdnlist(int fd, struct nlist *list)
{
	int result = 0;

	while (*list->n_name){
printf("__fdnlist(%d): name(%s) type(0x%x) other(%d) desc(%d) value(%d)\n", 
	fd, list->n_name, list->n_type, list->n_other, list->n_desc, list->n_value);
		result++;
		list++;
	}
	return(result);
}
void 
__tls_attach(const IMAGE_TLS_DIRECTORY *Image)
{
	size_t size = Image->EndAddressOfRawData - Image->StartAddressOfRawData;
	int index = *(int *)(Image->AddressOfIndex);

	__offsets[index].size = size;
	__offsets[index].address = Image->StartAddressOfRawData;
}
void 
__tls_copy(void *data[])
{
	tlsentry *entry = __offsets;
	int index = 0;

	/* When TLS is enabled, WinNT automatically copies the .data (.tls)
	 * sections for each new thread! Let's put the GOT data there first
	 * to make Windows copy parent data when forking (./arch/i386/fork.S).
	 */
	while (index < GOT_MAX){
		if (entry->size){
			memcpy(entry->address, data[index], entry->size);
		}
		entry++;
		index++;
	}
}
