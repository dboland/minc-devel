#include "SYS.h"

ENTRY(vfork)
	push %fs:0x2c
	call ___tls_copy
	add $4,%esp
	pop %ecx
	mov $SYS_vfork,%eax
	call ___kernel
	SYSCALL_ERROR
	push %ecx
	ret
