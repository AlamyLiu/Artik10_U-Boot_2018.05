/* SPDX-License-Identifier: GPL-2.0+ */
/*
 *  relocate - common relocation function for ARM U-Boot
 *
 *  Copyright (c) 2013  Albert ARIBAUD <albert.u.boot@aribaud.net>
 */

#include <asm-offsets.h>
#include <config.h>
#include <elf.h>
#include <linux/linkage.h>
#ifdef CONFIG_CPU_V7M
#include <asm/armv7m.h>
#endif

/*
 * Default/weak exception vectors relocation routine
 *
 * This routine covers the standard ARM cases: normal (0x00000000),
 * high (0xffff0000) and VBAR. SoCs which do not comply with any of
 * the standard cases must provide their own, strong, version.
 */

	.section	.text.relocate_vectors,"ax",%progbits
	.weak		relocate_vectors

ENTRY(relocate_vectors)
    /* Alamy: debugging messages */
    push {r0, lr}
	mov	r0, #0x3011
	bl	debug_msg_value
    pop {r0, lr}

#ifdef CONFIG_CPU_V7M
	/*
	 * On ARMv7-M we only have to write the new vector address
	 * to VTOR register.
	 */
	ldr	r0, [r9, #GD_RELOCADDR]	/* r0 = gd->relocaddr */
	ldr	r1, =V7M_SCB_BASE
	str	r0, [r1, V7M_SCB_VTOR]
#else
#ifdef CONFIG_HAS_VBAR
	/*
	 * If the ARM processor has the security extensions,
	 * use VBAR to relocate the exception vectors.
	 */
	ldr	r0, [r9, #GD_RELOCADDR]	/* r0 = gd->relocaddr */
	mcr     p15, 0, r0, c12, c0, 0  /* Set VBAR */
#else
	/*
	 * Copy the relocated exception vectors to the
	 * correct address
	 * CP15 c1 V bit gives us the location of the vectors:
	 * 0x00000000 or 0xFFFF0000.
	 */
	ldr	r0, [r9, #GD_RELOCADDR]	/* r0 = gd->relocaddr */
	mrc	p15, 0, r2, c1, c0, 0	/* V bit (bit[13]) in CP15 c1 */
	ands	r2, r2, #(1 << 13)
	ldreq	r1, =0x00000000		/* If V=0 */
	ldrne	r1, =0xFFFF0000		/* If V=1 */
	ldmia	r0!, {r2-r8,r10}
	stmia	r1!, {r2-r8,r10}
	ldmia	r0!, {r2-r8,r10}
	stmia	r1!, {r2-r8,r10}
#endif
#endif
    /* Alamy: debugging message */
    push {r0, lr}
	mov	r0, #0x3012
	bl	debug_msg_value
    pop {r0, lr}

	bx	lr

ENDPROC(relocate_vectors)

/*
 * void relocate_code(addr_moni)
 *
 * This function relocates the monitor code.
 *
 * NOTE:
 * To prevent the code below from containing references with an R_ARM_ABS32
 * relocation record type, we never refer to linker-defined symbols directly.
 * Instead, we declare literals which contain their relative location with
 * respect to relocate_code, and at run time, add relocate_code back to them.
 */

ENTRY(relocate_code)
    push {r0, lr}
    mov r8, lr
    mov r7, r0               /* relocation base */
	mov	r0, #0x3000
	bl	debug_msg_value
	bl	debug_msg_init
	bl	debug_msg
	ldr	r0, =__image_copy_start
	bl	debug_msg_value
	ldr	r0, =__image_copy_end
	bl	debug_msg_value
    mov r0, r7
    bl  debug_msg_value
    mov r0, r7
	ldr	r1, =__image_copy_start
	subs	r0, r0, r1		/* r0 <- relocation offset */
    bl  debug_msg_value
    mov r0, r8              /* lr: addr of 'here' */
    bl  debug_msg_value
    mov r0, r7
    pop {r0, lr}

	ldr	r1, =__image_copy_start	/* r1 <- SRC &__image_copy_start */
	subs	r4, r0, r1		/* r4 <- relocation offset */
	beq	relocate_done		/* skip relocation */
	ldr	r2, =__image_copy_end	/* r2 <- SRC &__image_copy_end */

copy_loop:
	ldmia	r1!, {r10-r11}		/* copy from source address [r1]    */
	stmia	r0!, {r10-r11}		/* copy to   target address [r0]    */
	cmp	r1, r2			/* until source end address [r2]    */
	blo	copy_loop

    /* Alamy: return addr 'here' memory dump */
#ifdef _ALAMY_DEBUG_BLOCK
    push    {r4, lr}
    mov     r0, r7
	ldr	    r1, =__image_copy_start
	subs	r0, r0, r1		    /* r0 <- relocation offset */
    subs    r0, r8, r0          /* r0 <- original LR */
    mov     r1, #0x80          /* Bytes */
    bl      debug_dump_mem
    mov     r0, r8              /* relocation LR */
    mov     r1, #0x80          /* Bytes */
    bl      debug_dump_mem
    pop     {r4, lr}
#endif

    /* Alamy: u-boot _image_copy_start memory dump */
    push    {r4, lr}
    mov     r7, r4
    ldr     r0, =__image_copy_start /* original memory dump */
    mov     r1, #0x80
    bl      debug_dump_mem
    ldr     r0, =__image_copy_start /* relocated memory dump */
    add     r0, r0, r7
    mov     r1, #0x80
    bl      debug_dump_mem
    pop     {r4, lr}

    push    {r4, lr}
	mov	    r0, #0x3005
	bl	    debug_msg_value
	ldr     r0, =__rel_dyn_start	/* r2 <- SRC &__rel_dyn_start */
	bl	    debug_msg_value
	ldr    	r0, =__rel_dyn_end	    /* r3 <- SRC &__rel_dyn_end */
	bl	    debug_msg_value

    mov     r0, r4                  /* relocation offset */
    bl      debug_msg_value

    mov     r1, #0x40
	ldr     r0, =__rel_dyn_start	/* r2 <- SRC &__rel_dyn_start */
    bl      debug_dump_mem
    pop     {r4, lr}


	/*
	 * fix .rel.dyn relocations
	 */
    mov r6, #0x0            /* Alamy: counter */
	ldr	r2, =__rel_dyn_start	/* r2 <- SRC &__rel_dyn_start */
	ldr	r3, =__rel_dyn_end	/* r3 <- SRC &__rel_dyn_end */
fixloop:
	ldmia	r2!, {r0-r1}		/* (r0,r1) <- (SRC location,fixup) */
	and	r1, r1, #0xff
	cmp	r1, #R_ARM_RELATIVE
	bne	fixnext

	/* relative fix: increase location by offset */
	add	r0, r0, r4
	ldr	r1, [r0]
	add	r1, r1, r4
	str	r1, [r0]

    /* Alamy: dump adjusted memory content */
#ifdef _ALAMY_DEBUG_ADJUST_ADDR
    add r6, r6, #1
    cmp r6, #0x100
    bge fixnext
    push {r0, r1, r2, r3}
    bl  debug_msg_value
    pop  {r0, r1, r2, r3}
    push {r0, r1, r2, r3}
    mov r0, r1
    bl  debug_msg_value
    pop  {r0, r1, r2, r3}
#endif

fixnext:
	cmp	r2, r3
	blo	fixloop

relocate_done:
    /* Alamy: debugging message */
    push    {r0, lr}
	mov	    r0, #0x3006
	bl	    debug_msg_value
    pop     {r0, lr}

#ifdef __XSCALE__
	/*
	 * On xscale, icache must be invalidated and write buffers drained,
	 * even with cache disabled - 4.2.7 of xscale core developer's manual
	 */
	mcr	p15, 0, r0, c7, c7, 0	/* invalidate icache */
	mcr	p15, 0, r0, c7, c10, 4	/* drain write buffer */
#endif

    push    {r0, lr}
    mov     r8, lr
	mov	    r0, #0x3008
	bl	    debug_msg_value
    mov     r0, r8
	bl	    debug_msg_value
    pop     {r0, lr}

	/* ARMv4- don't know bx lr but the assembler fails to see that */

#ifdef __ARM_ARCH_4__
	mov	pc, lr
#else
	bx	lr
#endif

ENDPROC(relocate_code)
