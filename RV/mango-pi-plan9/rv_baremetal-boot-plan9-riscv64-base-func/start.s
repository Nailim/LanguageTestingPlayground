
TEXT _start(SB), $0
    MOVW $stack+16384(SB), R2	// Set up stack pointer (top of reserved stack).
    MOVW $main(SB), R10	// Load address of main into R10.
    JMP (R10)			// Jump to main - a plain jump (C.JR-equivalent),
				// not a linked call, so unaffected by the
				// RV32C/RV64C C.JAL collision either way.
				// Both MOVWs above go through AUIPC+ADDI now
				// that optab.c routes AMOVW/C_LECON to the
				// PC-relative case (type 20) instead of the
				// broken absolute case (type 9) - see
				// plan9-riscv64-symbol-reloc-bug memory.

// REGARG/REGRET are both 8 (i.out.h) on this backend, shared by riscv and
// riscv64 - so R8-for-first-arg and R8-for-return below is confirmed by
// source, not just copied from the riscv32 callconv_test probe. What is
// NOT yet confirmed on real riscv64 hardware is whether a genuine linked
// CALL (as opposed to _start's plain JMP above) reaches its callee and
// returns correctly - that's what this folder's main.c exercises: PUT32
// (call, two args, no return use), GET32 (call, one arg, return value
// consumed by the caller), and dummy (call, one arg, void, in a loop).
TEXT PUT32(SB), $0
    MOVW val+4(FP), R11	// second argument (value) - passed on the stack
    MOVW R11, 0(R8)		// first argument (address) - passed in R8
    RET

TEXT GET32(SB), $0
    MOVW 0(R8), R8		// first (and only) argument - passed in R8,
				// result returned in the same register (REGRET==REGARG==8)
    RET

TEXT dummy(SB), $0
    RET                 // Empty function that does nothing

// Boot stack: reserved directly in BSS instead of computed via a linker
// script (Plan9's linker has no MEMORY/SECTIONS language). Referencing
// stack+16384(SB) gives the top of this 16KB region.
GLOBL stack(SB), $16384
