#include "../SYS.h"

ENTRY(_getlogin)
	pop %ecx
	mov $SYS_getlogin,%eax
	call ___kernel
	SYSCALL_ERROR
	push %ecx
	ret
