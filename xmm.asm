section .text

default rel
bits 64
global kernel_avx2_xmm

kernel_avx2_xmm:
    MOV R10D, [RSP+40] ; size
    PUSH R12
    PUSH R13
    PUSH R14
    PUSH R15

    MOV EAX, R10D
    AND EAX, 0x0003 ; check remainder
    SHR R10D, 2 ; number of groupes of 4
    XOR R11, R11 ; counter (number of elements)

    CMP R10D, 0 ; just jump to remainder if no group
    JE check_remainder

loop:
    VMOVDQU XMM0, [RCX + R11 * 4] ; vector A
    VMOVDQU XMM1, [RDX + R11 * 4] ; vector B

    VCMPPS XMM2, XMM0, XMM1, 1 ; compare if A is less than B
    VBLENDVPS XMM3, XMM0, XMM1, XMM2 ; blend the results based on the comparison
    VMOVDQU [R8 + R11 * 4], XMM3 ; store the bigger value
    VMOVMSKPS R13D, XMM2 ; take highest bit of each lane (4 lanes)
    XOR R12D, R12D ; counter for idx

idx:
    MOV R14D, R13D
    AND R14D, 0x0001 ; takes the value of the lowest bit
    LEA R15, [R9 + R11 * 4] ; address of the first idx pos in the group
    MOV [R15 + R12 * 4], R14D ; store the idx value
    SHR R13D, 1 ; shift to the right to get the next bit
    INC R12D ; increment the idx counter
    CMP R12D, 4
    JL idx

    ADD R11, 4
    DEC R10D
    JNZ loop

check_remainder:
    CMP EAX, 0
    JE end

    LEA R14, [RCX + R11 * 4] ; A
    LEA R15, [RDX + R11 * 4] ; B
    LEA R12, [R9 + R11 * 4] ; idx
    LEA R11, [R8 + R11 * 4] ; output
    XOR R13D, R13D ; counter

remainder_loop:
    MOVSS XMM0, [R14 + R13 * 4]
    MOVSS XMM1, [R15 + R13 * 4]

    UCOMISS XMM0, XMM1
    JAE greater_equal
    JB less

greater_equal:
    MOVSS [R11 + R13 * 4], XMM0
    MOV dword [R12 + R13 * 4], 0
    JMP next

less:
    MOVSS [R11 + R13 * 4], XMM1
    MOV dword [R12 + R13 * 4], 1

next:
    INC R13D
    DEC EAX
    JNZ remainder_loop

end:
    POP R15
    POP R14
    POP R13
    POP R12
    RET