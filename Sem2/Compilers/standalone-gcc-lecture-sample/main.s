	.file	"main.i"
	.text
	.section .rdata,"dr"
.LC2:
	.ascii "Result: %.2f\12\0"
.LC3:
	.ascii "Completion Status: %d\12\0"
	.text
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
.LFB114:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	movl	%ecx, 16(%rbp)
	movq	%rdx, 24(%rbp)
	call	__main
	movss	.LC0(%rip), %xmm0
	movss	%xmm0, -4(%rbp)
	movss	.LC1(%rip), %xmm0
	movss	%xmm0, -8(%rbp)
	movss	-8(%rbp), %xmm0
	movl	-4(%rbp), %eax
	movaps	%xmm0, %xmm1
	movd	%eax, %xmm0
	call	_Z16add_and_multiplyff
	movd	%xmm0, %eax
	movl	%eax, -12(%rbp)
	movq	.refptr.nCompletionStatus(%rip), %rax
	movl	$1, (%rax)
	pxor	%xmm0, %xmm0
	cvtss2sd	-12(%rbp), %xmm0
	movapd	%xmm0, %xmm1
	movapd	%xmm1, %xmm0
	movq	%xmm1, %rdx
	leaq	.LC2(%rip), %rax
	movapd	%xmm0, %xmm1
	movq	%rax, %rcx
	call	printf
	movq	.refptr.nCompletionStatus(%rip), %rax
	movl	(%rax), %eax
	leaq	.LC3(%rip), %rcx
	movl	%eax, %edx
	call	printf
	movl	$0, %eax
	addq	$48, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
	.align 4
.LC0:
	.long	1065353216
	.align 4
.LC1:
	.long	1084227584
	.def	__main;	.scl	2;	.type	32;	.endef
	.ident	"GCC: (Rev8, Built by MSYS2 project) 15.2.0"
	.def	_Z16add_and_multiplyff;	.scl	2;	.type	32;	.endef
	.def	printf;	.scl	2;	.type	32;	.endef
	.section	.rdata$.refptr.nCompletionStatus, "dr"
	.p2align	3, 0
	.globl	.refptr.nCompletionStatus
	.linkonce	discard
.refptr.nCompletionStatus:
	.quad	nCompletionStatus
