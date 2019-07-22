bits 32

section .text

global _Syscall
export _Syscall
_Syscall:
	mov eax, [esp + 4]
	mov ebx, [esp + 8]
	mov esi, [esp + 12]
	mov edi, [esp + 16]
	mov ecx, esp
	mov edx, .done
	sysenter
	.done:
	ret