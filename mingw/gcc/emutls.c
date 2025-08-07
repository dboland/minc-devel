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
 * 3. Neither the name of the copyright holder nor the names of its 
 *    contributors may be used to endorse or promote products derived 
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS 
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

#define NTAPI __attribute__((stdcall))
#define DLL_PROCESS_ATTACH    1

extern unsigned char __tls_start__;
extern unsigned char __tls_end__;

typedef void VOID, *PVOID, *LPVOID;
typedef unsigned long DWORD,*PDWORD,*LPDWORD;
typedef VOID (NTAPI *PIMAGE_TLS_CALLBACK)(PVOID,DWORD,PVOID);

static VOID NTAPI TlsMainCRTStartup(LPVOID hinstDLL, DWORD fdwReason, LPVOID lpvReserved);

typedef struct _IMAGE_TLS_DIRECTORY {
        LPVOID StartAddressOfRawData;
        LPVOID EndAddressOfRawData;
        LPVOID AddressOfIndex;
        LPVOID AddressOfCallBacks;
        LPVOID SizeOfZeroFill;
        LPVOID Characteristics;
} IMAGE_TLS_DIRECTORY,*PIMAGE_TLS_DIRECTORY;

typedef unsigned int word __attribute__((mode(word)));
typedef unsigned int pointer __attribute__((mode(pointer)));

struct __emutls_object
{
  word size;
  word align;
  union {
    pointer offset;
    void *ptr;
  } loc;
  void *templ;
};

unsigned char *__tls_get(int index);

DWORD __tls_index__ = -1;
PIMAGE_TLS_CALLBACK __tls_init__[2] = {&TlsMainCRTStartup, NULL};

const IMAGE_TLS_DIRECTORY _tls_used = {
	&__tls_start__,	/* start of tls data */
	&__tls_end__,	/* end of tls data */
	&__tls_index__,	/* offset into thread-local storage array */
	&__tls_init__,	/* pointer to call back array */
	0,			/* size of tls zero fill */
	0			/* characteristics */
};

void __tls_attach(const IMAGE_TLS_DIRECTORY *Image);

/************************************************************/

VOID NTAPI 
TlsMainCRTStartup(LPVOID lpvDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH){
		__tls_attach(&_tls_used);
	}
}
void *
__emutls_get_address(struct __emutls_object *obj)
{
	int offset = obj->templ - (void *)&__tls_start__;
	unsigned char *data = __tls_get(__tls_index__);

	return(data + offset);
}
__asm("___tls_get:\n"			\
	"mov 4(%esp),%ecx\n"		\
	"mov %fs:0x2c,%eax\n"		\
	"mov (%eax,%ecx,4),%eax\n"	\
	"ret\n"				\
);
