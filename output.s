	.globl	__main
__main:
	movl   $0x1, %eax
	movl   $0x1, %ecx
 	addl   %ecx, %eax
 	ret
