section .text

default rel
bits 64
global kernel_asm_x64

kernel_asm_x64:
	XOR R11, R11
	MOV R10D, [RSP+40]

loop:
	MOVSS XMM0, [RCX + R11 * 4]
	MOVSS XMM1, [RDX + R11 * 4]

	UCOMISS XMM0, XMM1
	JAE greater_equal
	JB less

greater_equal:
	MOVSS [R8 + R11 * 4], XMM0
	MOV dword [R9 + R11 * 4], 0
	JMP next

less:
	MOVSS [R8 + R11 * 4], XMM1
	MOV dword [R9 + R11 * 4], 1

next:
	INC R11
	DEC R10D
	JNZ loop

done:
	RET