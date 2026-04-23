; Simple 16-bit bootloader for x86_64
ORG 0x7C00

[bits 16]

start:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    
    ; Print loading
    mov si, msg
    call puts
    
    ; Load kernel from sector 2 (32KB after MBR to avoid overwriting)
    mov ah, 0x02
    mov al, 32
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, 0x80
    mov bx, 0x10000
    int 0x13
    jc fail
    
    ; Enable A20
    in al, 0x92
    or al, 2
    out 0x92, al
    
    ; Switch to protected mode then 64-bit
    cli
    lgdt [gdt_desc]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x08:.pmode

.pmode:
    [bits 32]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Jump to 64-bit kernel at 0x10000
    jmp 0x08:0x10000

fail:
    mov si, fail_msg
    call puts
    jmp $

puts:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp puts
.done:
    ret

msg db 'NYANIX x64...', 13, 10, 0
fail_msg db 'Error!', 13, 10, 0

; GDT
gdt:
    dw 0, 0, 0, 0
    dw 0xCF, 0x8F, 0x00, 0x00
    dw 0xCF, 0x8F, 0x00, 0x00
gdt_end:
gdt_desc:
    dw gdt_end - gdt - 1
    dd gdt

times 510-($-$$) db 0
dw 0xAA55