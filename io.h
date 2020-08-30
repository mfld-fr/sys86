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


