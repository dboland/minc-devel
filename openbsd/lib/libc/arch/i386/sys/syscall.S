#include "SYS.h"

ENTRY(syscall)
	pop %ecx
	pop %eax
	call ___kernel
	SYSCALL_ERROR
	push %ecx
	ret
