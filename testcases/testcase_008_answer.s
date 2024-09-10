	.globl	__main
__main:
	movl   $0x2, %eax
 	not    %eax
 	cmpl   $0, %eax
 	movl   $0, %eax
 	sete   %al
	movl   $0x3, %ebx
 	neg    %ebx
 	addl   %ebx, %eax
 	ret
