extern _RealRoutine

global _SetVideo

%define kernel_phys_offset 0xC0100000

section .data

setvideo_bin:                   incbin "..\\RealBlobs\\SetVideo.bin"
setvideo_end:
%define setvideo_size           setvideo_end - setvideo_bin

section .text

bits 32

_SetVideo:
	push ebx
	push esi
	push edi
	push ebp

	mov esi, setvideo_bin
	mov ecx, setvideo_size
	call _RealRoutine

	pop ebp
	pop edi
	pop esi
	pop ebx

	ret