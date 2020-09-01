
	.code16

	.text

	.global entry

entry:

	mov    %cs,%ax
	mov    %ax,%ds
	mov    %ax,%es

	xor    %dx,%dx
	mov    %ax,%ss  // auto CLI
	mov    %dx,%sp

	jmp    main
