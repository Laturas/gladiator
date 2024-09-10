	.globl	__main
__main:
	movl   $0x1, %eax
 	not    %eax
 	cmpl   $0, %eax
 	movl   $0, %eax
 	sete   %al
 	addl   $0x2, %eax
	movl   $0x1, %ebx
 	not    %ebx
 	addl   %ebx, %eax
	movl   $0x3, %ebx
 	cmpl   $0, %ebx
 	movl   $0, %ebx
 	sete   %al
 	addl   %ebx, %eax
 	addl   $0x1, %eax
	movl   $0x1, %ebx
 	neg    %ebx
 	addl   %ebx, %eax
 	ret
