.section .text
.global trap_vector

trap_vector:
    csrr a0, mcause       # Read cause of the exception
    csrr a1, mepc         # Read the exception PC
    csrr a2, mtval        # Read the faulting address or instruction
    call trap_handler     # Call the C handler: trap_handler(mcause, mepc, mtval)
	
.section .init

.option norvc

.type start, @function
.global start
start:
	.cfi_startproc
	
.option push
.option norelax
	la gp, global_pointer
.option pop
	
	/* Reset satp */
	csrw satp, zero
	
	/* Setup stack */
	la sp, stack_top
	
	/* Clear the BSS section */
	la t5, bss_start
	la t6, bss_end
bss_clear:
	la t0, trap_vector
	csrw mtvec, t0
	
	sd zero, (t5)
	addi t5, t5, 8
	bltu t5, t6, bss_clear
	
	la t0, kmain
	csrw mepc, t0
	
	/* Jump to kernel! */
	tail kmain
	
	.cfi_endproc

.end
