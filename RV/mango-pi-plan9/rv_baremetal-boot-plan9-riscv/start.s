
TEXT _start(SB), $0
    MOVW $stack+16384(SB), R2	// Set up stack pointer (top of reserved stack)
    MOVW $main(SB), R10		// Load address of main into R10
    JMP (R10)          		// Jump to main - a plain jump (C.JR), not a
					// linked call, so it's unaffected by the
					// RV32C/RV64C C.JAL collision below.

// PUT32/GET32/dummy were removed: calling them required a linked call,
// which ic/ia emit as C.JAL - an RV32C-only encoding that RV64C (what the
// D1's C906 core actually runs) redefines as C.ADDIW. On real hardware
// that silently turns every call into a no-op arithmetic instruction
// instead of a jump, with no trap - see main.c, which now does the GPIO
// accesses as direct pointer stores instead of calls.

// Boot stack: reserved directly in BSS instead of computed via a linker
// script (Plan9's linker has no MEMORY/SECTIONS language). Referencing
// stack+16384(SB) gives the top of this 16KB region.
GLOBL stack(SB), $16384
