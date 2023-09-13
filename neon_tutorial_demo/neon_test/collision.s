	.text
	.file	"collision.cpp"
	.globl	_Z12does_collideR6circleS0_     // -- Begin function _Z12does_collideR6circleS0_
	.p2align	2
	.type	_Z12does_collideR6circleS0_,@function
_Z12does_collideR6circleS0_:            // @_Z12does_collideR6circleS0_
	.cfi_startproc
// %bb.0:
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	str	x0, [sp, #24]
	str	x1, [sp, #16]
	ldr	x8, [sp, #24]
	ldr	s0, [x8, #4]
	ldr	x8, [sp, #16]
	ldr	s1, [x8, #4]
	fsub	s0, s0, s1
	str	s0, [sp, #12]
	ldr	x8, [sp, #24]
	ldr	s0, [x8, #8]
	ldr	x8, [sp, #16]
	ldr	s1, [x8, #8]
	fsub	s0, s0, s1
	str	s0, [sp, #8]
	ldr	s0, [sp, #12]
	ldr	s1, [sp, #12]
	ldr	s2, [sp, #8]
	ldr	s3, [sp, #8]
	fmul	s2, s2, s3
	fmadd	s0, s0, s1, s2
	str	s0, [sp, #4]
	ldr	x8, [sp, #24]
	ldr	s0, [x8]
	ldr	x8, [sp, #16]
	ldr	s1, [x8]
	fadd	s0, s0, s1
	ldr	x8, [sp, #24]
	ldr	s1, [x8]
	ldr	x8, [sp, #16]
	ldr	s2, [x8]
	fadd	s1, s1, s2
	fmul	s0, s0, s1
	str	s0, [sp]
	ldr	s0, [sp, #4]
	ldr	s1, [sp]
	fcmp	s0, s1
	cset	w8, ls
	and	w0, w8, #0x1
	add	sp, sp, #32
	ret
.Lfunc_end0:
	.size	_Z12does_collideR6circleS0_, .Lfunc_end0-_Z12does_collideR6circleS0_
	.cfi_endproc
                                        // -- End function
	.globl	_Z17does_collide_neonRK14circle_alignedS1_ // -- Begin function _Z17does_collide_neonRK14circle_alignedS1_
	.p2align	2
	.type	_Z17does_collide_neonRK14circle_alignedS1_,@function
_Z17does_collide_neonRK14circle_alignedS1_: // @_Z17does_collide_neonRK14circle_alignedS1_
	.cfi_startproc
// %bb.0:
	sub	sp, sp, #160
	.cfi_def_cfa_offset 160
	add	x8, sp, #136
	str	x0, [sp, #88]
	str	x1, [sp, #80]
	ldr	x9, [sp, #88]
	ldr	d0, [x9]
	str	d0, [sp, #64]
	ldr	d0, [sp, #64]
	str	d0, [sp, #56]
	ldr	d0, [sp, #56]
	str	d0, [sp, #72]
	ldr	x9, [sp, #80]
	ldr	d0, [x9]
	str	d0, [sp, #40]
	ldr	d0, [sp, #40]
	str	d0, [sp, #32]
	ldr	d0, [sp, #32]
	str	d0, [sp, #48]
	ldr	d1, [sp, #72]
	ldr	d0, [sp, #48]
	str	d1, [sp, #112]
	str	d0, [sp, #104]
	ldr	d0, [sp, #112]
	ldr	d1, [sp, #104]
	fsub	v0.2s, v0.2s, v1.2s
	str	d0, [sp, #96]
	ldr	d0, [sp, #96]
	str	d0, [sp, #24]
	ldr	d1, [sp, #24]
	ldr	d0, [sp, #24]
	str	d1, [x8]
	str	d0, [sp, #128]
	ldr	d0, [x8]
	ldr	d1, [sp, #128]
	fmul	v0.2s, v0.2s, v1.2s
	str	d0, [sp, #120]
	ldr	d0, [sp, #120]
	str	d0, [sp, #16]
	ldr	d0, [sp, #16]
	str	d0, [x8, #16]
	ldr	d0, [x8, #16]
	faddp	s0, v0.2s
	str	s0, [sp, #148]
	ldr	w8, [sp, #148]
	str	w8, [sp, #12]
	ldr	x8, [sp, #88]
	ldr	s0, [x8, #8]
	ldr	x8, [sp, #80]
	ldr	s1, [x8, #8]
	fadd	s0, s0, s1
	str	s0, [sp, #8]
	ldr	s0, [sp, #8]
	ldr	s1, [sp, #8]
	fmul	s0, s0, s1
	str	s0, [sp, #4]
	ldr	s0, [sp, #12]
	ldr	s1, [sp, #4]
	fcmp	s0, s1
	cset	w8, ls
	and	w0, w8, #0x1
	add	sp, sp, #160
	ret
.Lfunc_end1:
	.size	_Z17does_collide_neonRK14circle_alignedS1_, .Lfunc_end1-_Z17does_collide_neonRK14circle_alignedS1_
	.cfi_endproc
                                        // -- End function
	.globl	main                            // -- Begin function main
	.p2align	2
	.type	main,@function
main:                                   // @main
	.cfi_startproc
// %bb.0:
	stp	x29, x30, [sp, #-16]!           // 16-byte Folded Spill
	mov	x29, sp
	sub	x9, sp, #176
	and	sp, x9, #0xffffffffffffffc0
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	str	wzr, [sp, #172]
	add	x0, sp, #160
	fmov	s0, #2.00000000
	str	s0, [sp, #160]
	str	s0, [sp, #164]
	fmov	s0, #4.00000000
	str	s0, [sp, #168]
	add	x1, sp, #148
	fmov	s0, #1.00000000
	str	s0, [sp, #148]
	fmov	s1, #6.00000000
	str	s1, [sp, #152]
	str	s0, [sp, #156]
	bl	_Z12does_collideR6circleS0_
	tbz	w0, #0, .LBB2_2
	b	.LBB2_1
.LBB2_1:
	adrp	x0, .L.str
	add	x0, x0, :lo12:.L.str
	bl	printf
	b	.LBB2_3
.LBB2_2:
	adrp	x0, .L.str.1
	add	x0, x0, :lo12:.L.str.1
	bl	printf
	b	.LBB2_3
.LBB2_3:
	add	x0, sp, #64
	fmov	s0, #2.00000000
	str	s0, [sp, #72]
	str	s0, [sp, #64]
	fmov	s0, #4.00000000
	str	s0, [sp, #68]
	mov	x1, sp
	fmov	s0, #1.00000000
	str	s0, [sp, #8]
	fmov	s1, #6.00000000
	str	s1, [sp]
	str	s0, [sp, #4]
	bl	_Z17does_collide_neonRK14circle_alignedS1_
	tbz	w0, #0, .LBB2_5
	b	.LBB2_4
.LBB2_4:
	adrp	x0, .L.str
	add	x0, x0, :lo12:.L.str
	bl	printf
	b	.LBB2_6
.LBB2_5:
	adrp	x0, .L.str.1
	add	x0, x0, :lo12:.L.str.1
	bl	printf
	b	.LBB2_6
.LBB2_6:
	mov	w0, wzr
	mov	sp, x29
	ldp	x29, x30, [sp], #16             // 16-byte Folded Reload
	ret
.Lfunc_end2:
	.size	main, .Lfunc_end2-main
	.cfi_endproc
                                        // -- End function
	.type	.L.str,@object                  // @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"Circles collide\n"
	.size	.L.str, 17

	.type	.L.str.1,@object                // @.str.1
.L.str.1:
	.asciz	"Circles do not collide\n"
	.size	.L.str.1, 24

	.ident	"Android (9352603, based on r450784d1) clang version 14.0.7 (https://android.googlesource.com/toolchain/llvm-project 4c603efb0cca074e9238af8b4106c30add4418f6)"
	.section	".note.GNU-stack","",@progbits
