; Nyanix x86_64 bootloader
; - VBE mode 0x118 (1024x768 16bpp linear)
; - Identity-maps 0-4GB with 2MB pages (PML4 at 0x70000)
; - Enters 64-bit long mode, jumps to kernel at 0x10000

[bits 16]
ORG 0x7C00

start:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; Save boot drive
    mov [boot_drive], dl

    ; ── VBE: set mode 0x118 (1024x768 16bpp + linear) ─────────────────────
    mov ax, 0x4F02
    mov bx, 0x4118          ; mode 0x118 | LFB bit (0x4000)
    int 0x10
    cmp ax, 0x004F
    jne fail

    ; ── VBE: get mode info → 0x8000 ────────────────────────────────────────
    mov ax, 0x4F01
    mov cx, 0x118
    mov di, 0x8000
    int 0x10

    ; ── Load kernel (sectors 2..33 → 0x10000) ──────────────────────────────
    mov ah, 0x02
    mov al, 64              ; 64 sectors = 32KB
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, [boot_drive]
    mov bx, 0x1000
    mov es, bx
    xor bx, bx
    int 0x13
    jc fail

    ; ── Enable A20 ─────────────────────────────────────────────────────────
    in  al, 0x92
    or  al, 2
    out 0x92, al

    ; ── Build page tables at 0x70000 (identity map 0-4GB, 2MB pages) ───────
    ; Clear 6 pages (PML4 + PDPT + 4 PDs)
    mov edi, 0x70000
    mov ecx, 0x6000 / 4
    xor eax, eax
    rep stosd

    ; PML4[0] → PDPT at 0x71000 (P|RW)
    mov dword [0x70000], 0x71000 | 0x03
    mov dword [0x70004], 0

    ; PDPT[0..3] → PDs at 0x72000..0x75000
    mov dword [0x71000], 0x72000 | 0x03
    mov dword [0x71010], 0x73000 | 0x03
    mov dword [0x71020], 0x74000 | 0x03
    mov dword [0x71030], 0x75000 | 0x03

    ; Fill PDs — each entry maps 2MB (PS|RW|P = 0x83)
    mov edi, 0x72000
    mov eax, 0x00000083     ; first 2MB
    mov ecx, 4 * 512        ; 4 PDs × 512 entries
.fill_pd:
    mov [edi], eax
    mov dword [edi+4], 0
    add eax, 0x200000
    add edi, 8
    loop .fill_pd

    ; ── Switch to protected mode ────────────────────────────────────────────
    cli
    lgdt [gdt64_desc]

    mov eax, cr0
    or  eax, 1
    mov cr0, eax

    jmp 0x08:.pmode32

[bits 32]
.pmode32:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; CR3 = PML4
    mov eax, 0x70000
    mov cr3, eax

    ; Enable PAE (CR4 bit 5)
    mov eax, cr4
    or  eax, (1 << 5)
    mov cr4, eax

    ; Set LME in EFER (MSR 0xC0000080 bit 8)
    mov ecx, 0xC0000080
    rdmsr
    or  eax, (1 << 8)
    wrmsr

    ; Enable paging + protected mode (CR0 PG|PE)
    mov eax, cr0
    or  eax, (1 << 31) | 1
    mov cr0, eax

    ; Far jump into 64-bit CS
    jmp 0x18:.lmode64

[bits 64]
.lmode64:
    mov ax, 0x20
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Stack at 2MB
    mov rsp, 0x200000

    ; Jump to kernel entry
    mov rax, 0x10000
    jmp rax

fail:
    mov si, fail_msg
    call puts16
    jmp $

puts16:
    lodsb
    or  al, al
    jz  .done
    mov ah, 0x0E
    int 0x10
    jmp puts16
.done:
    ret

boot_drive db 0
fail_msg   db 'BOOT FAIL', 13, 10, 0

; ── GDT ────────────────────────────────────────────────────────────────────
align 8
gdt64:
    ; Entry 0: null
    dq 0
    ; Entry 1: 32-bit code (selector 0x08) — used for pmode32 transition
    dw 0xFFFF, 0x0000, 0x9A00, 0x00CF
    ; Entry 2: 32-bit data (selector 0x10)
    dw 0xFFFF, 0x0000, 0x9200, 0x00CF
    ; Entry 3: 64-bit code (selector 0x18) — L=1, D=0, G=1 → byte6=0xAF
    dw 0xFFFF, 0x0000, 0x9A00, 0x00AF
    ; Entry 4: 64-bit data (selector 0x20)
    dw 0xFFFF, 0x0000, 0x9200, 0x00AF
gdt64_end:
gdt64_desc:
    dw gdt64_end - gdt64 - 1
    dd gdt64

times 510-($-$$) db 0
dw 0xAA55
