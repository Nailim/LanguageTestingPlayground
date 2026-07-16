
TEXT _start(SB), $0
    MOVW $stack+16384(SB), R2	// Set up stack pointer (top of reserved stack).
					// UNVERIFIED: riscv64 addresses are 64-bit: MOVW
					// may only set the low 32 bits of R2, leaving
					// the upper bits undefined. Check whether this
					// port has a wider move (MOVV/MOVD-style) before
					// trusting this on real hardware.
    MOVW $main(SB), R10		// Load address of main into R10 (same 64-bit
					// concern as above).
    JMP (R10)          		// Jump to main - a plain jump (C.JR-equivalent),
					// not a linked call, so unaffected by the
					// RV32C/RV64C C.JAL collision either way.

// PUT32/GET32's argument convention below is COPIED UNVERIFIED from
// objtype=riscv (RV32): first argument in R8, further arguments at
// FP-relative offsets - confirmed there via callconv_test/put.c compiled
// with `ic -S` (see ../rv_baremetal-boot-plan9/callconv_test/put.c).
// riscv64 may not use the same convention. Recompile callconv_test/put.c
// here with objtype=riscv64 and `ic -S` and compare before trusting this.
TEXT PUT32(SB), $0
    MOVW val+4(FP), R11	// second argument (value) - passed on the stack
    MOVW R11, 0(R8)		// first argument (address) - passed in R8
    RET

// Return-value register also unverified - see the same caveat in the
// riscv32 version's start.s history.
TEXT GET32(SB), $0
    MOVW 0(R8), R8		// first (and only) argument - passed in R8
    RET

TEXT dummy(SB), $0
    RET                 // Empty function that does nothing

// Boot stack: reserved directly in BSS instead of computed via a linker
// script (Plan9's linker has no MEMORY/SECTIONS language). Referencing
// stack+16384(SB) gives the top of this 16KB region.
GLOBL stack(SB), $16384
