; thx mint

org 0x1000
bits 32
; Save stack
mov dword [saved_stack], esp

; Save arg for later
mov dword [arg], ebx

; Save pmode IDT
sidt [pmode_idt]

; Save pmode GDT
sgdt [pmode_gdt]

; Load real mode IDT
lidt [real_idt]

; save cr3
mov eax, cr3
mov dword [cr3_reg], eax

; Load 16-bit segments
jmp 0x28:pmode16
pmode16:
bits 16
mov ax, 0x30
mov ds, ax
mov es, ax
mov ss, ax
mov fs, ax
mov gs, ax

; Leave pmode
mov eax, cr0
and eax, ~(0x80000001)
mov cr0, eax

; Load real mode segments
jmp 0x0000:real_mode
real_mode:
xor ax, ax
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax
mov ax, 0x1000
mov ss, ax
mov esp, 0xFFF0

; Retrieve arg
mov ebx, dword [arg]

; Call actual routine
call 0x8000

; Load intermediate bootstrap GDT
lgdt [gdt_ptr]

; load cr3
mov eax, dword [cr3_reg]
mov cr3, eax

; Enter pmode
mov eax, cr0
or eax, 0x80000001
mov cr0, eax

; Load pmode segments
jmp 0x08:.pmode
.pmode:
bits 32
mov ax, 0x10
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax
mov ss, ax

; Load pmode IDT
lidt [pmode_idt]

; Load pmode GDT
lgdt [pmode_gdt]

; Restore stack
mov esp, dword [saved_stack]

; Return
ret

data:

align 4
pmode_idt:
dw 0
dd 0

align 4
real_idt:
dw 0x3FF
dd 0

align 4
pmode_gdt:
dw 0
dd 0

arg dd 0
cr3_reg dd 0
saved_stack dd 0

align 16

gdt_ptr:
dw .gdt_end - .gdt_start - 1  ; GDT size
dq .gdt_start                 ; GDT start

align 16
.gdt_start:

; Null descriptor (required)
.null_descriptor:
dw 0x0000           ; Limit
dw 0x0000           ; Base (low 16 bits)
db 0x00             ; Base (mid 8 bits)
db 00000000b        ; Access
db 00000000b        ; Granularity
db 0x00             ; Base (high 8 bits)

; 32 bit mode kernel
.kernel_code_32:
dw 0xFFFF           ; Limit
dw 0x0000           ; Base (low 16 bits)
db 0x00             ; Base (mid 8 bits)
db 10011010b        ; Access
db 11001111b        ; Granularity
db 0x00             ; Base (high 8 bits)

.kernel_data_32:
dw 0xFFFF           ; Limit
dw 0x0000           ; Base (low 16 bits)
db 0x00             ; Base (mid 8 bits)
db 10010010b        ; Access
db 11001111b        ; Granularity
db 0x00             ; Base (high 8 bits)

.gdt_end: