bits 32

section .text

global __aulldvrm
__aulldvrm:
	push esi
	xor esi, esi
	mov eax, [esp + 20]
	mov edx, [esp + 16]
	or eax, eax
	jge .L1
	neg eax
	neg edx
	sbb eax, 0
	inc esi
	.L1: push eax
	push edx
	mov eax, [esp + 20]
	mov edx, [esp + 16]
	or eax, eax
	jge .L2
	or esi, 4
	neg eax
	neg edx
	sbb eax, 0
	inc esi
	.L2: push eax
	push edx
	call __aulldvrm
	shr esi, 1
	jnc short .L3
	neg edx
	neg eax
	sbb edx, 0
	.L3: shr esi, 2
	jnc short .L4
	neg ebx
	neg ecx
	sbb ebx, 0
	.L4: pop esi
	ret 16

global __aulldiv
__aulldiv:
	push ebp
	mov ebp, esp
	push ebx
	push dword [ebp + 20]
	push dword [ebp + 16]
	push dword [ebp + 12]
	push dword [ebp + 8]
	call __aulldvrm
	pop ebx
	leave
	ret 16

global __aullrem
__aullrem:
	push ebp
	mov ebp, esp
	push ebx
	push dword [ebp + 20]
	push dword [ebp + 16]
	push dword [ebp + 12]
	push dword [ebp + 8]
	call __aulldvrm
	mov edx, ebx
	mov eax, ecx
	pop ebx
	leave
	ret 16
