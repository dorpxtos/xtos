org 0x8000
bits 16

%define VESA_MODE 0115h

mov ax, 4F01h
mov cx, VESA_MODE | 4000h
mov di, 9000h
int 10h

mov ax, 4F02h
mov bx, VESA_MODE | 4000h
int 10h

ret