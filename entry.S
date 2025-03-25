.section .text
.global trap_vector

trap_vector:
    csrr a0, mcause       # Read cause of the exception
    csrr a1, mepc         # Read the exception PC
    csrr a2, mtval        # Read the faulting address or instruction
    call trap_handler      # Call the C handler: trap_handler(mcause, mepc, mtval)
    ret                    # Return from the trap

.section .init

.option norvc

.type start, @function
.global start
start:
    .cfi_startproc

    /* Setup stack */
    la sp, stack_top

    /* Clear the BSS section */
    la t5, bss_start
    la t6, bss_end
bss_clear:	
    sd zero, (t5)         # Clear the BSS section
    addi t5, t5, 8
    bltu t5, t6, bss_clear

    la x4, trap_vector
    csrw mtvec, x4
	
    /* Jump to kernel! */
    la t0, kmain
    jalr t0                # Jump to kmain

    .cfi_endproc

.end
