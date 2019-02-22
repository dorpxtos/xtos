.686p
.MODEL FLAT, C
.STACK
.DATA
.CODE

EXTRN gdtr : FWORD

LoadGdt PROC
	lgdt FWORD PTR [gdtr]
	jmp OFFSET @@loadsegs
@@loadsegs:
	mov ax, 10h
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	ret
LoadGdt ENDP

END