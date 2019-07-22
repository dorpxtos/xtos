bits 32

STACK_TOP EQU 15728624
KERNEL_MEMORY_BASE EQU 0xC0000000
kernel_cr3 EQU 0x800000
kernel_cr3.pd EQU kernel_cr3
kernel_cr3.pt EQU kernel_cr3.pd + 4096
kernel_cr3.end EQU kernel_cr3.pt + (4096 * 8)

section .text

extern _KernelMain
global _Entry
_Entry:
	mov esp, STACK_TOP
	xor ebp, ebp
	push eax
	add ebx, KERNEL_MEMORY_BASE
	push ebx

	; zero out page tables
	xor eax, eax
	mov edi, kernel_cr3
	mov ecx, (kernel_cr3.end - kernel_cr3) / 4
	rep stosd

	; set up page tables
	mov eax, 03h
	mov edi, kernel_cr3.pt
	mov ecx, 1024 * 8
.L0:
	stosd
	add eax, 1000h
	loop .L0

	; set up page directories
	mov eax, kernel_cr3.pt
	or eax, 03h
	mov edi, kernel_cr3.pd
	mov ecx, 8
.L1:
	stosd
	add eax, 1000h
	loop .L1

	mov eax, kernel_cr3.pt
	or eax, 03h
	mov edi, kernel_cr3.pd + (640 + 128) * 4
	mov ecx, 8
.L2:
	stosd
	add eax, 1000h
	loop .L2

	mov eax, kernel_cr3
	mov cr3, eax

	mov eax, cr0
	or eax, 1 << 31
	mov cr0, eax

	add esp, KERNEL_MEMORY_BASE
	mov eax, _KernelMain
	call eax
	jmp Hang

Hang:
	cli
	hlt
	jmp Hang