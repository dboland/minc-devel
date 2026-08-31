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

#include "arch_types.h"

.file "fork.S"
.global _sys_fork, _sys_vfork
.text

# aliasen (constants)

return = 0
origin = 4
size   = 8
source = 12
offset = 16
dest   = 20

_fork_enter:
	push %fs:0x2c			# NT_TIB.ThreadLocalStoragePointer
	call _proc_fork_enter		# create WIN_THREAD_STRUCT
	add $8,%esp
	# set up WIN_THREAD_STRUCT with parent data
	pop %ecx			# save return addr, thereby aligning stack
	mov %ecx,return(%eax)
	mov %fs:0x4,%ecx		# NT_TIB.StackBase
	mov %ecx,origin(%eax)
	mov %ecx,size(%eax)
	sub %esp,size(%eax)
	mov %ebp,source(%eax)
	mov %ebp,offset(%eax)
	sub %esp,offset(%eax)
	# build frame
	push return(%eax)
	enter $0,$0
	# pass WIN_THREAD_STRUCT to child
	push %eax
	push $_fork_leave
	call _proc_fork
	add $8,%esp
	leave
	ret

_fork_leave:
	mov 4(%esp),%ecx		# WIN_THREAD_STRUCT
	enter $0,$0
	# allocate stack space
	sub size(%ecx),%esp
	mov %ebp,dest(%ecx)
	push %ecx
	call _task_copy
	pop %ecx
	# restore pointers
	mov dest(%ecx),%esp
	mov %esp,%ebp
	add offset(%ecx),%ebp
	push return(%ecx)
	push %ecx
	call _proc_fork_leave		# destroy WIN_THREAD_STRUCT
	add $4,%esp
	mov $0,%eax
	ret

.def _sys_fork; .scl 2; .type 32; .endef

_sys_fork:
	push $0
	jmp _fork_enter
	nop

.def _sys_vfork; .scl 2; .type 32; .endef

_sys_vfork:
	push $WIN_PS_PPWAIT
	jmp _fork_enter
	nop
