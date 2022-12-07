ORG 0x7c00 
BITS 16

start:
	mov si, message
	call print
	jmp $
print:
	mov bx, 0
.loop:
	lodsb
	cmp al,0
	je .done
	call print_char
	jmp .loop
.done:
	ret
print_char:
	mov ah, 0x0e ; Command For Writing Character
	int 0x10     ; Interrupt to BIOS which will do the write for us
	ret	     ; return from function

message: db 'Hello World!', 0

times 510- ($ - $$) db 0 ; Fill 510 bytes with 0x00
dw 0xAA55                ; Boot Signature in Little Endian
