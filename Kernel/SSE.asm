bits 32

section .text

global _SSEInit
_SSEInit:
	mov eax, cr4
	or ax, 3 << 9
	mov cr4, eax
	ret