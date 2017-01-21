  ;mov    dx,#io_port1_mode
  ;in     ax,dx
  ;mov    bx,#serial_mask
  ;or     ax,bx
  ;out    dx,ax

  ;mov    dx,#io_port1_dir
  ;in     ax,dx
  ;mov    bx,#serial_mask
  ;not    bx
  ;and    ax,bx
  ;out    dx,ax

  ;mov    dx,#io_port1_data
  ;in     ax,dx
  ;mov    bx,#serial_mask
  ;not    bx
  ;and    ax,bx
  ;mov    bx,#$0100
  ;or     ax,bx
  ;out    dx,ax

  ;mov    dx,#io_print_com
  ;in     al,dx
  ;and    al,#$F0
  ;mov    bl,#$04
  ;or     al,bl
  ;out    dx,al
