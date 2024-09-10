	.globl	__main
__main:
	movl   $0x1, %eax
 	cmpl   $0, %eax
 	movl   $0, %eax
 	sete   %al
 	addl   $0x1, %eax
 	addl   $0x1, %eax
	movl   $0x1, %ebx
 	cmpl   $0, %ebx
 	movl   $0, %ebx
 	sete   %al
 	addl   %ebx, %eax
 	addl   $0x1, %eax
 	addl   $0x1, %eax
 	ret
