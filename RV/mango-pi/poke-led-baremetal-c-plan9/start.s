TEXT _start(SB), $-4

    // Optional: set stack pointer
    MOVW $0x42100000, R2   // SP

    // Configure PD18 as output (PD_CFG2 = 0x02000098)
    MOVW $0x02000098, R10
    MOVW $0x0FFFF1FF, R11
    MOVW R11, 0(R10)

    // Delay loop
    MOVW $100000, R12
delay:
    ADDW $-1, R12
    BNE R12, R0, delay

    // Set PD18 high (PD_DATA = 0x020000A0)
    MOVW $0x020000A0, R10
    MOVW $0x00040000, R11
    MOVW R11, 0(R10)

loop:
    JMP loop
