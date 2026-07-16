
TEXT _start(SB), $0
    MOVW $setSB(SB), R3	// Initialize SB (R3): required before any code
				// touches a "small" global/string constant.
				// Those compile to a single SB-relative ADDI
				// (C_SECON in the linker's aclass()) instead of
				// the AUIPC+ADDI path large constants use, and
				// R3 is never otherwise set up on a bare-metal
				// boot - standard Plan9 process startup (rt0)
				// does this same MOVW before calling main, we
				// just never had a reason to need it until code
				// referenced small data (uart_puts's strings).
    MOVW $stack+16384(SB), R2	// Set up stack pointer (top of reserved stack).
    MOVW $main(SB), R10	// Load address of main into R10.
    JMP (R10)			// Plain jump into main - see
				// rv_baremetal-boot-plan9-riscv64-func for why
				// both MOVWs above and a real linked CALL are
				// both confirmed working on riscv64 hardware.

// REGARG/REGRET are both 8 (i.out.h), confirmed working end to end
// (arguments, void calls, and return values) in
// rv_baremetal-boot-plan9-riscv64-func.
TEXT PUT32(SB), $0
    MOVW val+4(FP), R11	// second argument (value) - passed on the stack
    MOVW R11, 0(R8)		// first argument (address) - passed in R8
    RET

TEXT GET32(SB), $0
    MOVW 0(R8), R8		// first (and only) argument - passed in R8,
				// result returned in the same register (REGRET==REGARG==8)
    RET

TEXT dummy(SB), $0
    RET                 // Empty function that does nothing - used as a
			// loop body for busy-wait delays.

// Boot stack: reserved directly in BSS instead of computed via a linker
// script (Plan9's linker has no MEMORY/SECTIONS language). Referencing
// stack+16384(SB) gives the top of this 16KB region.
GLOBL stack(SB), $16384
