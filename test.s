;------------------------------------------------------------------------------
; TEST - Low level test
;------------------------------------------------------------------------------


; Register offsets

io_print_com    EQU $031A

io_timer0_mode  EQU $FF56
io_timer1_mode  EQU $FF5E
io_timer2_mode  EQU $FF66

io_port0_data   EQU $FF74

io_port1_mode   EQU $FF76
io_port1_dir    EQU $FF78
io_port1_data   EQU $FF7A


; PIO masks

port0_mask      EQU $FE33
port1_mask      EQU $67F2

led1_mask       EQU $0002
serial_mask     EQU $6700


; Segments

seg_stage       equ $1000
seg_prog1       equ $F800


;------------------------------------------------------------------------------

	.TEXT

_entry:

	push    cx
	push    dx
	push    di
	push    ds
	push    es

	; erase flash block

	mov     dx,#seg_prog1
	xor     cx,cx
	mov     ah,#1
	int     $61
	or      al,al
	jnz     flash_fail

	; check block erased

	mov     ax,#seg_prog1
	mov     es,ax
	xor     di,di

	mov     cx,#$800
        mov     ax,#$FFFF
	cld
	repe
	scasw
	jnz     flash_fail

	; write block

	mov     ax,#seg_stage
	mov     ds,ax
	xor     si,si

	mov     dx,#seg_prog1
	xor     cx,cx
	mov     ah,#2
	int     $61
	or      al,al
	jnz     flash_fail

	; check block write

	xor     si,si
	xor	di,di
	mov     cx,#$800
	repe
	cmpsw
	jnz     flash_fail

	xor     ax,ax
	jmp     flash_exit

flash_fail:

	mov     ax,#1

flash_exit:

	pop     es
	pop     ds
	pop     di
	pop     dx
	pop     cx
	retf

	ENTRY  _entry

	END

;------------------------------------------------------------------------------
