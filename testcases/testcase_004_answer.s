	.globl	__main
__main:
	movl   $0x1, %eax
 	addl   $0x1, %eax
 	addl   $0x1, %eax
 	addl   $0x1, %eax
 	addl   $0x1, %eax
	movl   $0x1, %ebx
 	not    %ebx
 	not    %ebx
 	addl   %ebx, %eax
 	ret
