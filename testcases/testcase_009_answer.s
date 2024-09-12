	.globl	__main
__main:
	movl   $0x1, %eax
 	cmpl   $0, %eax
 	movl   $0, %eax
 	sete   %al
 	addl   $0x2, %eax
	movl   $0x3, %ebx
 	subl   %eax, %ebx
	movl   %ebx, %eax
 	ret
