// Register save/restore mechanics test - see README.
// R4 is the permanent scratch/base-pointer register for the whole
// sequence (never gets a test pattern, never checked in main.c) -
// R0(zero)/R1(link)/R2(sp)/R3(sb) are all needed for this very
// function to keep running, so none of them are free to hold an
// arbitrary test pattern either; R5-R31 (27 registers) are the
// actual test set. Each gets value -n (sign-extended): a proven-safe
// immediate (small negative constants already validated in
// rv_baremetal-boot-plan9-riscv64-test-branches), distinct per
// register, with all upper 32 bits set - a save/restore that
// silently narrows to 32 bits shows up as a wrong high half, not a
// coincidentally-still-correct low half.
TEXT regsave_test(SB), $0
    MOVW $-5, R5
    MOVW $-6, R6
    MOVW $-7, R7
    MOVW $-8, R8
    MOVW $-9, R9
    MOVW $-10, R10
    MOVW $-11, R11
    MOVW $-12, R12
    MOVW $-13, R13
    MOVW $-14, R14
    MOVW $-15, R15
    MOVW $-16, R16
    MOVW $-17, R17
    MOVW $-18, R18
    MOVW $-19, R19
    MOVW $-20, R20
    MOVW $-21, R21
    MOVW $-22, R22
    MOVW $-23, R23
    MOVW $-24, R24
    MOVW $-25, R25
    MOVW $-26, R26
    MOVW $-27, R27
    MOVW $-28, R28
    MOVW $-29, R29
    MOVW $-30, R30
    MOVW $-31, R31

    // save x0..x31 into regbuf1 (32*8 = 256 bytes)
    MOVW $regbuf1(SB), R4
    MOV R0, 0(R4)
    MOV R1, 8(R4)
    MOV R2, 16(R4)
    MOV R3, 24(R4)
    MOV R4, 32(R4)
    MOV R5, 40(R4)
    MOV R6, 48(R4)
    MOV R7, 56(R4)
    MOV R8, 64(R4)
    MOV R9, 72(R4)
    MOV R10, 80(R4)
    MOV R11, 88(R4)
    MOV R12, 96(R4)
    MOV R13, 104(R4)
    MOV R14, 112(R4)
    MOV R15, 120(R4)
    MOV R16, 128(R4)
    MOV R17, 136(R4)
    MOV R18, 144(R4)
    MOV R19, 152(R4)
    MOV R20, 160(R4)
    MOV R21, 168(R4)
    MOV R22, 176(R4)
    MOV R23, 184(R4)
    MOV R24, 192(R4)
    MOV R25, 200(R4)
    MOV R26, 208(R4)
    MOV R27, 216(R4)
    MOV R28, 224(R4)
    MOV R29, 232(R4)
    MOV R30, 240(R4)
    MOV R31, 248(R4)

    // clobber R5..R31 with different values (proves restore below
    // really reloads from memory, not coincidence)
    MOVW $10, R5
    MOVW $12, R6
    MOVW $14, R7
    MOVW $16, R8
    MOVW $18, R9
    MOVW $20, R10
    MOVW $22, R11
    MOVW $24, R12
    MOVW $26, R13
    MOVW $28, R14
    MOVW $30, R15
    MOVW $32, R16
    MOVW $34, R17
    MOVW $36, R18
    MOVW $38, R19
    MOVW $40, R20
    MOVW $42, R21
    MOVW $44, R22
    MOVW $46, R23
    MOVW $48, R24
    MOVW $50, R25
    MOVW $52, R26
    MOVW $54, R27
    MOVW $56, R28
    MOVW $58, R29
    MOVW $60, R30
    MOVW $62, R31

    // restore R5..R31 from regbuf1 (R4 still points at it)
    MOV 40(R4), R5
    MOV 48(R4), R6
    MOV 56(R4), R7
    MOV 64(R4), R8
    MOV 72(R4), R9
    MOV 80(R4), R10
    MOV 88(R4), R11
    MOV 96(R4), R12
    MOV 104(R4), R13
    MOV 112(R4), R14
    MOV 120(R4), R15
    MOV 128(R4), R16
    MOV 136(R4), R17
    MOV 144(R4), R18
    MOV 152(R4), R19
    MOV 160(R4), R20
    MOV 168(R4), R21
    MOV 176(R4), R22
    MOV 184(R4), R23
    MOV 192(R4), R24
    MOV 200(R4), R25
    MOV 208(R4), R26
    MOV 216(R4), R27
    MOV 224(R4), R28
    MOV 232(R4), R29
    MOV 240(R4), R30
    MOV 248(R4), R31

    // re-save x0..x31 into regbuf2, proving restore+resave is
    // consistent with the original save, not just self-consistent
    MOVW $regbuf2(SB), R4
    MOV R0, 0(R4)
    MOV R1, 8(R4)
    MOV R2, 16(R4)
    MOV R3, 24(R4)
    MOV R4, 32(R4)
    MOV R5, 40(R4)
    MOV R6, 48(R4)
    MOV R7, 56(R4)
    MOV R8, 64(R4)
    MOV R9, 72(R4)
    MOV R10, 80(R4)
    MOV R11, 88(R4)
    MOV R12, 96(R4)
    MOV R13, 104(R4)
    MOV R14, 112(R4)
    MOV R15, 120(R4)
    MOV R16, 128(R4)
    MOV R17, 136(R4)
    MOV R18, 144(R4)
    MOV R19, 152(R4)
    MOV R20, 160(R4)
    MOV R21, 168(R4)
    MOV R22, 176(R4)
    MOV R23, 184(R4)
    MOV R24, 192(R4)
    MOV R25, 200(R4)
    MOV R26, 208(R4)
    MOV R27, 216(R4)
    MOV R28, 224(R4)
    MOV R29, 232(R4)
    MOV R30, 240(R4)
    MOV R31, 248(R4)

    RET

GLOBL regbuf1(SB), $256
GLOBL regbuf2(SB), $256
