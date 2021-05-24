OUTPUT_FORMAT(binary)

SECTIONS {
	.text : { *(.text*) *(.rodata) *(.data) }
	.bss  : { _bss_begin = . ; *(.bss) *(COMMON) }
	_free_begin = . ;
	/DISCARD/ : { *(.comment) }
	}
