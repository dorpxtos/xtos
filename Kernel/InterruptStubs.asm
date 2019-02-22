.686p
.MODEL FLAT, C
.STACK
.DATA
.CODE

EXTRN IsrHandler:near
EXTRN IrqHandler:near
EXTRN SyscallHandler:near

Isr0 PROC
	push 0
	push 0
	jmp IsrStub
Isr0 ENDP

Isr1 PROC
	push 0
	push 1
	jmp IsrStub
Isr1 ENDP

Isr2 PROC
	push 0
	push 2
	jmp IsrStub
Isr2 ENDP

Isr3 PROC
	push 0
	push 3
	jmp IsrStub
Isr3 ENDP

Isr4 PROC
	push 0
	push 4
	jmp IsrStub
Isr4 ENDP

Isr5 PROC
	push 0
	push 5
	jmp IsrStub
Isr5 ENDP

Isr6 PROC
	push 0
	push 6
	jmp IsrStub
Isr6 ENDP

Isr7 PROC
	push 0
	push 7
	jmp IsrStub
Isr7 ENDP

Isr8 PROC
	push 8
	jmp IsrStub
Isr8 ENDP

Isr9 PROC
	push 0
	push 9
	jmp IsrStub
Isr9 ENDP

Isr10 PROC
	push 0
	push 10
	jmp IsrStub
Isr10 ENDP

Isr11 PROC
	push 11
	jmp IsrStub
Isr11 ENDP

Isr12 PROC
	push 12
	jmp IsrStub
Isr12 ENDP

Isr13 PROC
	push 13
	jmp IsrStub
Isr13 ENDP

Isr14 PROC
	push 14
	jmp IsrStub
Isr14 ENDP

Isr15 PROC
	push 0
	push 15
	jmp IsrStub
Isr15 ENDP

Isr16 PROC
	push 0
	push 16
	jmp IsrStub
Isr16 ENDP

Isr17 PROC
	push 0
	push 17
	jmp IsrStub
Isr17 ENDP

Isr18 PROC
	push 0
	push 18
	jmp IsrStub
Isr18 ENDP

Isr19 PROC
	push 0
	push 19
	jmp IsrStub
Isr19 ENDP

Isr20 PROC
	push 0
	push 20
	jmp IsrStub
Isr20 ENDP

Isr21 PROC
	push 0
	push 21
	jmp IsrStub
Isr21 ENDP

Isr22 PROC
	push 0
	push 22
	jmp IsrStub
Isr22 ENDP

Isr23 PROC
	push 0
	push 23
	jmp IsrStub
Isr23 ENDP

Isr24 PROC
	push 0
	push 24
	jmp IsrStub
Isr24 ENDP

Isr25 PROC
	push 0
	push 25
	jmp IsrStub
Isr25 ENDP

Isr26 PROC
	push 0
	push 26
	jmp IsrStub
Isr26 ENDP

Isr27 PROC
	push 0
	push 27
	jmp IsrStub
Isr27 ENDP

Isr28 PROC
	push 0
	push 28
	jmp IsrStub
Isr28 ENDP

Isr29 PROC
	push 0
	push 29
	jmp IsrStub
Isr29 ENDP

Isr30 PROC
	push 0
	push 30
	jmp IsrStub
Isr30 ENDP

Isr31 PROC
	push 0
	push 31
	jmp IsrStub
Isr31 ENDP

Irq0 PROC
	push 0
	push 32
	jmp IrqStub
Irq0 ENDP

Irq1 PROC
	push 0
	push 33
	jmp IrqStub
Irq1 ENDP

Irq2 PROC
	push 0
	push 34
	jmp IrqStub
Irq2 ENDP

Irq3 PROC
	push 0
	push 35
	jmp IrqStub
Irq3 ENDP

Irq4 PROC
	push 0
	push 36
	jmp IrqStub
Irq4 ENDP

Irq5 PROC
	push 0
	push 37
	jmp IrqStub
Irq5 ENDP

Irq6 PROC
	push 0
	push 38
	jmp IrqStub
Irq6 ENDP

Irq7 PROC
	push 0
	push 39
	jmp IrqStub
Irq7 ENDP

Irq8 PROC
	push 0
	push 40
	jmp IrqStub
Irq8 ENDP

Irq9 PROC
	push 0
	push 41
	jmp IrqStub
Irq9 ENDP

Irq10 PROC
	push 0
	push 42
	jmp IrqStub
Irq10 ENDP

Irq11 PROC
	push 0
	push 43
	jmp IrqStub
Irq11 ENDP

Irq12 PROC
	push 0
	push 44
	jmp IrqStub
Irq12 ENDP

Irq13 PROC
	push 0
	push 45
	jmp IrqStub
Irq13 ENDP

Irq14 PROC
	push 0
	push 46
	jmp IrqStub
Irq14 ENDP

Irq15 PROC
	push 0
	push 47
	jmp IrqStub
Irq15 ENDP

syscallReturnValue DD 0

SyscallStub PROC
	push 0
	push 128
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

	mov eax, SyscallHandler
	call eax
	mov syscallReturnValue, eax

	pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	pop eax
	popad
	mov eax, syscallReturnValue
	add esp, 8
	iretd
SyscallStub ENDP

IsrStub PROC
	pusha
	mov eax, esp
	push eax
	mov eax, IsrHandler
	call eax
	pop eax
	popad
	add esp, 8
	iretd
IsrStub ENDP

IrqStub PROC
	pusha
	mov eax, esp
	push eax
	mov eax, IrqHandler
	call eax
	pop eax
	popad
	add esp, 8
	iretd
IrqStub ENDP

END