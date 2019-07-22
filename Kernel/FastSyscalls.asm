bits 32

section .data
fastSyscallReturnValue dd 0

section .text

extern _SyscallHandler

_FastSyscallEntry:
	cli
	push ss		; 4
	push edx	; 8
	pushf		; 12
	push cs		; 16
	push ecx	; 20
	push 0		; 24
	push 128	; 28
	pusha

	mov eax, esp
	push eax

	xor eax, eax
	mov ax, ds
	push eax
	mov ax, 10h
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	mov eax, _SyscallHandler
	call eax
	mov dword [fastSyscallReturnValue], eax

	pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	pop eax
	popa
	mov eax, dword [fastSyscallReturnValue]
	add esp, 28
	sti
	sysexit

global _FastSyscallsInit
_FastSyscallsInit:
	; CS
	mov ecx, 174h
	mov eax, 08h
	mov edx, 0
	wrmsr
	
	; ESP
	mov ecx, 175h
	mov eax, fastSyscallStackEnd
	mov edx, 0
	wrmsr

	; EIP
	mov ecx, 176h
	mov eax, _FastSyscallEntry
	mov edx, 0
	wrmsr

	; Done!
	ret

section .bss
fastSyscallStack: resb 2046
fastSyscallStackEnd: