ENTRY(start);
 
. = 0x80000000;
 
SECTIONS {
	/* Include entry point at start of binary */
	.text : ALIGN(4) {
		*(.init);
		*(.text);
	}
	.bss : ALIGN(4) {
		PROVIDE(bss_start = .);
		*(.bss);
		PROVIDE(bss_end = .);
	}

	.stack (NOLOAD) : ALIGN(4) {
		PROVIDE(stack_top = . + 4096);
		PROVIDE(global_pointer = . + 8192);
	}
	.rodata : ALIGN(4) {
		*(.rodata);
	}
	.data : ALIGN(4) {
		*(.data);
	}
}
