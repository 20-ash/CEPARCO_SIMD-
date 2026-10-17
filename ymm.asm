section .text

default rel
bits 64
global kernel_avx2_ymm

kernel_avx2_ymm:
    MOV R10D, [RSP+40] ; size
    PUSH R12
    PUSH R13
    PUSH R14
    PUSH R15

    MOV EAX, R10D
    AND EAX, 0x0007 ; check remainder
    SHR R10D, 3 ; number of groups of 8
    XOR R11, R11 ; counter (number of elements)

    CMP R10D, 0 ; just jump to remainder if no group
    JE check_remainder

loop:
    VMOVDQU YMM0, [RCX + R11 * 4] ; vector A
    VMOVDQU YMM1, [RDX + R11 * 4] ; vector B

    VCMPPS YMM2, YMM0, YMM1, 1 ; compare if A is less than B
    VMAXPS  YMM3, YMM0, YMM1      ; max(A,B)
    VMOVUPS [R8 + R11 * 4], YMM3    ; store C
    VPSRLD  YMM2, YMM2, 31        ; each 32 bit lane shifts 31 bits to the right then get the remaining one bit and pad the upper portions with 0s
    VMOVDQU [R9 + R11 * 4], YMM2    ; store idx
    ADD R11, 8
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