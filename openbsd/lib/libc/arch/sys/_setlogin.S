#include "../SYS.h"

ENTRY(_setlogin)
	pop %ecx
	mov $SYS_setlogin,%eax
	call ___kernel
	SYSCALL_ERROR
	push %ecx
	ret
