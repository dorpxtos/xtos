; thx mint

global _RealRoutine

section .data

%define real_init_size  real_init_end - real_init
real_init:              incbin "..\\RealBlobs\\Real.bin"
real_init_end:

section .text

bits 32

_RealRoutine:
	; ESI = routine location
	; ECX = routine size
    
	push esi
	push ecx

	; Real mode init blob to 0000:1000
	mov esi, real_init
	mov edi, 0x1000
	mov ecx, real_init_size
.L0:
	movsb
	loop .L0
    
	; Routine's blob to 0000:8000
	pop ecx
	pop esi
	mov edi, 0x8000
.L1:
	movsb
	loop .L1
    
	; Call module
	mov eax, 0x1000
	call eax
    
	ret