// Non-commutative 3-operand arithmetic tests - see README. ADD/SUB/SLL/
// SLT/SLTU/SRL/SRA all share optab.c's case-0/OP_R path and the exact
// same "oprrr rreg ',' sreg ',' rreg" grammar rule bug #4 (see
// PLAN9_RISCV64_LESSONS_LEARNED.md) fixed for the branch family - the
// fix covers these too, but they were never actually exercised, only
// inferred to share the fix by reading the grammar. Commutative ops
// (ADD/AND/OR/XOR/MUL) would have hidden the bug the same way BEQ/BNE
// did; these are the non-commutative ones where a lingering operand-
// order mistake would be visible.
//
// Each test writes its result back into R8 (both a source operand and
// the destination register - legal, sources are read before the
// write-back), matching REGRET==REGARG==8.

TEXT sub_test(SB), $0
    MOVW $10, R8
    MOVW $3, R9
    SUB R8, R9, R8	// 10 - 3 = 7. If this were still backwards
			// (3 - 10), the result would be -7.
    RET

TEXT slt_true(SB), $0
    MOVW $3, R8
    MOVW $7, R9
    SLT R8, R9, R8	// 3 < 7 signed: 1
    RET

TEXT slt_false(SB), $0
    MOVW $7, R8
    MOVW $3, R9
    SLT R8, R9, R8	// 7 < 3 signed: 0
    RET

TEXT sltu_true(SB), $0
    MOVW $3, R8
    MOVW $7, R9
    SLTU R8, R9, R8	// 3 < 7 unsigned: 1
    RET

TEXT sltu_false(SB), $0
    MOVW $7, R8
    MOVW $3, R9
    SLTU R8, R9, R8	// 7 < 3 unsigned: 0
    RET

TEXT sltu_sign(SB), $0
    MOVW $1, R8
    MOVW $-1, R9
    SLTU R8, R9, R8	// 1 < (huge unsigned) -1: 1. Signed would give 0
			// (1 < -1 is false) - the same signed/unsigned
			// distinguishing case branchtest.s already used.
    RET

TEXT sll_test(SB), $0
    MOVW $1, R8
    MOVW $4, R9
    SLL R8, R9, R8	// 1 << 4 = 16
    RET

TEXT srl_test(SB), $0
    MOVW $-1, R8
    MOVW $60, R9
    SRL R8, R9, R8	// logical: all-ones shifted right 60, zero-filled
			// from the top - only the low 4 bits survive: 15
    RET

TEXT sra_test(SB), $0
    MOVW $-16, R8
    MOVW $2, R9
    SRA R8, R9, R8	// arithmetic: sign-preserving shift, -16 >> 2 = -4
    RET
