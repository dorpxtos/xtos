.686p
.MODEL FLAT, C
.STACK
.DATA
.CODE

EXTRN KernelMain:near

stackTop EQU 15728624

Entry PROC
	mov esp, stackTop
	mov ebp, 0
	push eax
	push ebx
	call KernelMain
	jmp Hang
Entry ENDP

Hang PROC
	cli
	hlt
	jmp Hang
Hang ENDP

END