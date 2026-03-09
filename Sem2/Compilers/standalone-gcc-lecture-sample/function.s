	.file	"function.i"
	.text
	.globl	nCompletionStatus
	.bss
	.align 4
nCompletionStatus:
	.space 4
	.text
	.globl	_Z3addff
	.def	_Z3addff;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z3addff
_Z3addff:
.LFB0:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$16, %rsp
	.seh_stackalloc	16
	.seh_endprologue
	movss	%xmm0, 16(%rbp)
	movss	%xmm1, 24(%rbp)
	movss	16(%rbp), %xmm0
	addss	24(%rbp), %xmm0
	movss	%xmm0, -4(%rbp)
	movss	-4(%rbp), %xmm0
	addq	$16, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z16add_and_multiplyff
	.def	_Z16add_and_multiplyff;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z16add_and_multiplyff
_Z16add_and_multiplyff:
.LFB1:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	movss	%xmm0, 16(%rbp)
	movss	%xmm1, 24(%rbp)
	movss	24(%rbp), %xmm0
	movl	16(%rbp), %eax
	movaps	%xmm0, %xmm1
	movd	%eax, %xmm0
	call	_Z3addff
	movd	%xmm0, %eax
	movl	%eax, -4(%rbp)
	movss	-4(%rbp), %xmm1
	movss	.LC0(%rip), %xmm0
	mulss	%xmm1, %xmm0
	movss	%xmm0, -4(%rbp)
	movss	-4(%rbp), %xmm0
	addq	$48, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
	.align 4
.LC0:
	.long	1077936128
	.ident	"GCC: (Rev8, Built by MSYS2 project) 15.2.0"
