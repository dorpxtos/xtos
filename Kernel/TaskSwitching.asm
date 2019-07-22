.686p
.MODEL FLAT, C
.STACK
.DATA
.CODE

newCr3 DD 0

TaskSpinup PROC
	mov ebx, [esp+4]
	mov eax, [esp+8]
	mov [newCr3], eax
	mov eax, [ebx]
	mov ecx, [ebx+8]
	mov edx, [ebx+12]
	mov esi, [ebx+16]
	mov edi, [ebx+20]
	mov ebp, [ebx+24]

	push [ebx+56]
	push [ebx+28]
	push [ebx+60]
	push [ebx+36]
	push [ebx+32]

	push [ebx+44]
	pop es
	push [ebx+48]
	pop fs
	push [ebx+52]
	pop gs
	push [ebx+40]
	pop ds
	push [ebx+4]
	mov ebx, [newCr3]
	mov cr3, ebx
	pop ebx

	iretd
TaskSpinup ENDP

END