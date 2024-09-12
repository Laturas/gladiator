	.globl	__main
__main:
	movl   $0x3, %eax
 	neg    %eax
	movl   $0x2, %ebx
 	not    %ebx
 	cmpl   $0, %ebx
 	movl   $0, %ebx
 	sete   %al
 	addl   %ebx, %eax
 	ret
