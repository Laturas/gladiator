	.globl	__main
__main:
	movl   $0x2, %eax
 	imul   $0x3, %eax
	movl   $0x5, %ebx
 	imul   $0x2, %ebx
 	subl   $0x6, %ebx
 	addl   %ebx, %eax
 	ret
