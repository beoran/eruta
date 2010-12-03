	.file	"orharun.c"
	.text
	.p2align 4,,15
.globl or_runtime_init
	.type	or_runtime_init, @function
or_runtime_init:
	pushl	%ebp
	movl	$1, %edx
	movl	%esp, %ebp
	movl	8(%ebp), %eax
	movl	$1024, (%eax)
	movl	$0, 8(%eax)
	.p2align 4,,7
.L2:
	movl	$0, 8(%eax,%edx,4)
	addl	$1, %edx
	cmpl	$1025, %edx
	jne	.L2
	popl	%ebp
	ret
	.size	or_runtime_init, .-or_runtime_init
	.p2align 4,,15
.globl or_runtime_pushnext
	.type	or_runtime_pushnext, @function
or_runtime_pushnext:
	pushl	%ebp
	movl	%esp, %ebp
	movl	8(%ebp), %eax
	movl	12(%ebp), %ecx
	movl	4(%eax), %edx
	addl	$1, %edx
	cmpl	$1023, %edx
	movl	%edx, 4(%eax)
	jg	.L13
	movl	%ecx, 12(%eax,%edx,4)
	movl	%ecx, 8(%eax)
	popl	%ebp
	ret
.L13:
	movl	$1023, 4(%eax)
	xorl	%eax, %eax
	popl	%ebp
	ret
	.size	or_runtime_pushnext, .-or_runtime_pushnext
	.p2align 4,,15
.globl or_runtime_popnext
	.type	or_runtime_popnext, @function
or_runtime_popnext:
	pushl	%ebp
	movl	%esp, %ebp
	movl	8(%ebp), %edx
	movl	4(%edx), %eax
	subl	$1, %eax
	testl	%eax, %eax
	movl	%eax, 4(%edx)
	js	.L19
	movl	12(%edx,%eax,4), %eax
	movl	%eax, 8(%edx)
	movl	%edx, %eax
	popl	%ebp
	ret
.L19:
	movl	$0, 8(%edx)
	xorl	%eax, %eax
	popl	%ebp
	ret
	.size	or_runtime_popnext, .-or_runtime_popnext
	.p2align 4,,15
.globl or_runtime_next
	.type	or_runtime_next, @function
or_runtime_next:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	%ebx, -12(%ebp)
	movl	8(%ebp), %ebx
	movl	%edi, -4(%ebp)
	movl	12(%ebp), %eax
	movl	%esi, -8(%ebp)
	movl	16(%ebp), %edi
	movl	8(%ebx), %esi
	movl	%eax, -16(%ebp)
	testl	%esi, %esi
	je	.L25
	movl	%ebx, (%esp)
	call	or_runtime_popnext
	movl	-16(%ebp), %ecx
	movl	%edi, 16(%ebp)
	movl	-4(%ebp), %edi
	movl	%ebx, 8(%ebp)
	movl	-12(%ebp), %ebx
	movl	%ecx, 12(%ebp)
	movl	%esi, %ecx
	movl	-8(%ebp), %esi
	movl	%ebp, %esp
	popl	%ebp
	jmp	*%ecx
	.p2align 4,,7
.L25:
	movl	-12(%ebp), %ebx
	xorl	%eax, %eax
	movl	-8(%ebp), %esi
	movl	-4(%ebp), %edi
	movl	%ebp, %esp
	popl	%ebp
	ret
	.size	or_runtime_next, .-or_runtime_next
	.p2align 4,,15
.globl or_runtime_run
	.type	or_runtime_run, @function
or_runtime_run:
	pushl	%ebp
	xorl	%eax, %eax
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$12, %esp
	movl	8(%ebp), %ebx
	movl	12(%ebp), %edi
	movl	16(%ebp), %esi
	movl	8(%ebx), %ecx
	testl	%ecx, %ecx
	je	.L29
	.p2align 4,,7
.L31:
	movl	%esi, 8(%esp)
	movl	%edi, 4(%esp)
	movl	%ebx, (%esp)
	call	or_runtime_next
	movl	8(%ebx), %edx
	testl	%edx, %edx
	jne	.L31
.L29:
	addl	$12, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
	.size	or_runtime_run, .-or_runtime_run
	.p2align 4,,15
.globl main
	.type	main, @function
main:
	leal	4(%esp), %ecx
	andl	$-16, %esp
	pushl	-4(%ecx)
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4136, %esp
	movl	%ebx, -4(%ebp)
	leal	-4116(%ebp), %ebx
	movl	%ecx, -8(%ebp)
	movl	%ebx, (%esp)
	call	or_runtime_init
	movl	%ebx, (%esp)
	movl	$hello, 4(%esp)
	call	or_runtime_pushnext
	movl	%ebx, (%esp)
	movl	$0, 8(%esp)
	movl	$0, 4(%esp)
	call	or_runtime_run
	movl	-8(%ebp), %ecx
	xorl	%eax, %eax
	movl	-4(%ebp), %ebx
	movl	%ebp, %esp
	popl	%ebp
	leal	-4(%ecx), %esp
	ret
	.size	main, .-main
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"Hello, orhalrun!"
	.text
	.p2align 4,,15
.globl hello
	.type	hello, @function
hello:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	movl	$.LC0, (%esp)
	call	puts
	movl	8(%ebp), %eax
	movl	$world, 4(%esp)
	movl	%eax, (%esp)
	call	or_runtime_pushnext
	xorl	%eax, %eax
	leave
	ret
	.size	hello, .-hello
	.section	.rodata.str1.1
.LC1:
	.string	"Hello, world orhalrun!"
	.text
	.p2align 4,,15
.globl world
	.type	world, @function
world:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	movl	$.LC1, (%esp)
	call	puts
	xorl	%eax, %eax
	leave
	ret
	.size	world, .-world
	.ident	"GCC: (GNU) 4.1.2 20071124 (Red Hat 4.1.2-42)"
	.section	.note.GNU-stack,"",@progbits
