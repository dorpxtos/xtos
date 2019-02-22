.686p
.MODEL FLAT, C
.STACK
.DATA
.CODE

kernelPagemap equ 800000h

PageMapInit PROC
	; zero out page tables
	xor eax, eax
	mov edi, kernelPagemap
	mov ecx, 1024 * (8 + 1)
	rep stosd

	; set up page tables
	mov eax, 3
	mov edi, kernelPagemap+4096
	mov ecx, 1024 * 8
	@@L0:
	stosd
	add eax, 1000h
	loop @@L0

	; set up page directories
	mov eax, (kernelPagemap + 4096)
	or eax, 3
	mov edi, kernelPagemap
	mov ecx, 8
	@@L1:
	stosd
	add eax, 1000h
	loop @@L1

	mov eax, kernelPagemap
	mov cr3, eax

	mov eax, cr0
	or eax, 1 SHL 31
	mov cr0, eax

	ret
PageMapInit ENDP

END