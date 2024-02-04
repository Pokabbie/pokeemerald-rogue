	.include "asm/macros.inc"
	thumb_func_start RandomPCG32
RandomPCG32:: @function written by ax6, credit to him
	push {r4-r6}
	@load the fixed 64-bit factor (r5:r4) and the RNG state (seed in r1, state in r3:r2)
	ldr r6, =.Ldata
	ldmia r6!, {r4-r5}
	ldmia r0!, {r1-r3}
	@convert the seed into an addend in r6:r1 (addend = seed * 2 + 1)
	lsr r6, r1, #31
	cmp r6, r6 @force set carry
	adc r1, r1
	@precalculate the high products
	mul r5, r2
	add r6, r5
	mov r5, r4
	mul r5, r3
	add r6, r5
	@precalculate the rotation count...
	lsr r5, r3, #27
	@...and switch modes
	bx pc

	.arm
	@finish the state update
	umlal r1, r6, r2, r4
	@do the xorshift
	mov r4, r3, lsr #13
	eor r4, r3, lsl #5
	eor r4, r2, lsr #27
	@store the updated state
	stmdb r0, {r1, r6}
	@rotate the xorshifted value, pop and finish
	mov r0, r4, ror r5
	pop {r4-r6}
	bx lr

	.thumb
.Ldata:
	.word 0x4c957f2d
	.word 0x5851f42d
	thumb_func_end RandomPCG32