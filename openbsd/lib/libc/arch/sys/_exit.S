#include "../SYS.h"

ENTRY(_exit)
	pop %ecx
	mov $SYS_exit,%eax
	call ___kernel
	SYSCALL_ERROR
	push %ecx
	ret
