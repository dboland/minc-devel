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

#include "../../kern/syscall.h"

.file "syscall.S"
.global ___kernel, _sys_syscall
.text

.def ___kernel; .scl 2; .type 32; .endef

___kernel:
	# setup call_t struct
	cmp $SYS_MAXSYSCALL,%eax
	jge 1f
	push %eax		# syscall code
	push $-1		# WIN_TASK
	push %ecx
	push %ebx
	push %esi
	push %edi
	push $0			# result low
	push $0			# result high
	call _syscall_enter
	call *%eax
	mov %edx,(%esp)
	mov %eax,4(%esp)
	call _syscall_leave
	pop %edx		# result high
	pop %eax		# result low
	pop %edi
	pop %esi
	pop %ebx
	pop %ecx
	add $4,%esp		# WIN_TASK
	add $4,%esp		# code
1:
	ret

.def _sys_syscall; .scl 2; .type 32; .endef

_sys_syscall:
	pop %ecx
	pop %eax
	call ___kernel
	push %ecx
	ret

.def _sys__threxit; .scl 2; .type 32; .endef

_sys__threxit:
	pop %ecx
	mov $SYS___threxit,%eax
	call ___kernel
	push %ecx
	ret
