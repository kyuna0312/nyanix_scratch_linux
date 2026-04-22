[bits 32]
START:
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