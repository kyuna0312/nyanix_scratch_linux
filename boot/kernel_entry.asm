[bits 32]
global _start
_start:
    ; Enable SSE (CR4.OSFXSR + CR4.OSXMMEXCPT, clear EM, set MP in CR0)
    mov eax, cr0
    and eax, ~(1 << 2)   ; clear CR0.EM (no x87 emulation)
    or  eax,  (1 << 1)   ; set   CR0.MP (monitor coprocessor)
    mov cr0, eax
    mov eax, cr4
    or  eax,  (3 << 9)   ; set CR4.OSFXSR | CR4.OSXMMEXCPT
    mov cr4, eax

[extern start]
    call start
    jmp $

extern _idt, HandleISR0, HandleISR1, HandleISR12
global isr0, isr1, isr12
global LoadIDT

IDTDesc:
	dw 2048
	dd _idt

isr0:
	pusha
	call HandleISR0
	popa
	iret

isr1:
	pusha
	call HandleISR1
	popa
	iret

isr12:
	pusha
	call HandleISR12
	popa
	iret

LoadIDT:
	lidt[IDTDesc]
	sti
	ret