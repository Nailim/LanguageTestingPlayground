// Hand-written branch instruction tests. See README for the full
// hypothesis - short version: rv_baremetal-boot-plan9-riscv64-diag's
// kernel-skeleton work found that "BGE R9, R11, target" (3-operand
// register form) assembles with the operands compiled in the OPPOSITE
// order from what was written, and a trailing unconditional "JMP" back
// to a label preceded by a conditional branch got rewritten into a
// different conditional branch entirely - neither of those was
// previously exercised anywhere in this project (every earlier test/
// driver used either straight-line code or C-level if/switch/while
// compiled BY ic, not hand-written ja branch instructions). Every
// function here returns its result in R8 (REGRET), taking no arguments,
// with all operand values hardcoded as immediates - deliberately
// avoiding any dependency on the (separately, already proven) multi-
// argument calling convention, so a wrong result here can only mean the
// branch/loop instruction itself misbehaved, nothing else.
//
// Naming: <mnemonic>_<case>. "_true" or a bare case name means the
// branch is expected to be TAKEN (function returns 1). "_false" or
// "_rev" (operands reversed from the corresponding true/non-rev case)
// means NOT taken (returns 0). Each register-vs-register comparison
// test also has a real, independently useful property: if the operand
// order compiled backwards (like the original BGE bug), or if a signed
// comparison were accidentally computed as unsigned or vice versa, the
// result would flip - these are not arbitrary values, "3 vs 7" and
// "-1 vs 1" were chosen specifically to make both classes of bug
// visible as a wrong PASS/FAIL rather than an accidental correct answer.

// ---- BEQ/BNE (3-operand register form; sign-independent) ----

TEXT beq_true(SB), $0
    MOVW $5, R8
    MOVW $5, R9
    BEQ R8, R9, beq_true_yes
    MOVW $0, R8
    RET
beq_true_yes:
    MOVW $1, R8
    RET

TEXT beq_false(SB), $0
    MOVW $3, R8
    MOVW $7, R9
    BEQ R8, R9, beq_false_yes
    MOVW $0, R8
    RET
beq_false_yes:
    MOVW $1, R8
    RET

TEXT bne_true(SB), $0
    MOVW $3, R8
    MOVW $7, R9
    BNE R8, R9, bne_true_yes
    MOVW $0, R8
    RET
bne_true_yes:
    MOVW $1, R8
    RET

TEXT bne_false(SB), $0
    MOVW $5, R8
    MOVW $5, R9
    BNE R8, R9, bne_false_yes
    MOVW $0, R8
    RET
bne_false_yes:
    MOVW $1, R8
    RET

// ---- BLT (3-operand, signed) ----

TEXT blt_basic(SB), $0
    MOVW $3, R8
    MOVW $7, R9
    BLT R8, R9, blt_basic_yes	// 3 < 7 signed: true
    MOVW $0, R8
    RET
blt_basic_yes:
    MOVW $1, R8
    RET

TEXT blt_sign(SB), $0
    MOVW $-1, R8
    MOVW $1, R9
    BLT R8, R9, blt_sign_yes	// -1 < 1 signed: true. If this were done
				// unsigned instead, -1 (huge) < 1 would be
				// false - this is the signed/unsigned
				// distinguishing case.
    MOVW $0, R8
    RET
blt_sign_yes:
    MOVW $1, R8
    RET

TEXT blt_sign_rev(SB), $0
    MOVW $1, R8
    MOVW $-1, R9
    BLT R8, R9, blt_sign_rev_yes	// 1 < -1 signed: false
    MOVW $0, R8
    RET
blt_sign_rev_yes:
    MOVW $1, R8
    RET

// ---- BGE (3-operand, signed) ----

TEXT bge_basic(SB), $0
    MOVW $7, R8
    MOVW $3, R9
    BGE R8, R9, bge_basic_yes	// 7 >= 3 signed: true
    MOVW $0, R8
    RET
bge_basic_yes:
    MOVW $1, R8
    RET

TEXT bge_sign(SB), $0
    MOVW $1, R8
    MOVW $-1, R9
    BGE R8, R9, bge_sign_yes	// 1 >= -1 signed: true. Unsigned would
				// make this false (1 >= huge).
    MOVW $0, R8
    RET
bge_sign_yes:
    MOVW $1, R8
    RET

TEXT bge_sign_rev(SB), $0
    MOVW $-1, R8
    MOVW $1, R9
    BGE R8, R9, bge_sign_rev_yes	// -1 >= 1 signed: false
    MOVW $0, R8
    RET
bge_sign_rev_yes:
    MOVW $1, R8
    RET

// ---- BLTU (3-operand, unsigned) ----

TEXT bltu_basic(SB), $0
    MOVW $3, R8
    MOVW $7, R9
    BLTU R8, R9, bltu_basic_yes	// 3 < 7 unsigned: true
    MOVW $0, R8
    RET
bltu_basic_yes:
    MOVW $1, R8
    RET

TEXT bltu_sign(SB), $0
    MOVW $1, R8
    MOVW $-1, R9
    BLTU R8, R9, bltu_sign_yes	// 1 < (huge unsigned) -1: true. Signed
				// would make this false (1 < -1 is false).
    MOVW $0, R8
    RET
bltu_sign_yes:
    MOVW $1, R8
    RET

TEXT bltu_sign_rev(SB), $0
    MOVW $-1, R8
    MOVW $1, R9
    BLTU R8, R9, bltu_sign_rev_yes	// (huge) < 1 unsigned: false
    MOVW $0, R8
    RET
bltu_sign_rev_yes:
    MOVW $1, R8
    RET

// ---- BGEU (3-operand, unsigned) ----

TEXT bgeu_basic(SB), $0
    MOVW $7, R8
    MOVW $3, R9
    BGEU R8, R9, bgeu_basic_yes	// 7 >= 3 unsigned: true
    MOVW $0, R8
    RET
bgeu_basic_yes:
    MOVW $1, R8
    RET

TEXT bgeu_sign(SB), $0
    MOVW $-1, R8
    MOVW $1, R9
    BGEU R8, R9, bgeu_sign_yes	// (huge) >= 1 unsigned: true. Signed
				// would make this false (-1 >= 1 is false).
    MOVW $0, R8
    RET
bgeu_sign_yes:
    MOVW $1, R8
    RET

TEXT bgeu_sign_rev(SB), $0
    MOVW $1, R8
    MOVW $-1, R9
    BGEU R8, R9, bgeu_sign_rev_yes	// 1 >= (huge) unsigned: false
    MOVW $0, R8
    RET
bgeu_sign_rev_yes:
    MOVW $1, R8
    RET

// ---- 2-operand (vs implicit zero register) forms ----
// A DIFFERENT assembler grammar rule than the 3-operand forms above
// (cmd/ia/a.y: "LBEQ rreg ',' rel" vs "LBEQ rreg ',' sreg ',' rel") -
// worth testing separately since there's no guarantee it has the same
// operand-order behavior as the 3-operand form.

TEXT beqz_true(SB), $0
    MOVW $0, R8
    BEQ R8, beqz_true_yes
    MOVW $0, R8
    RET
beqz_true_yes:
    MOVW $1, R8
    RET

TEXT beqz_false(SB), $0
    MOVW $5, R8
    BEQ R8, beqz_false_yes
    MOVW $0, R8
    RET
beqz_false_yes:
    MOVW $1, R8
    RET

TEXT bnez_true(SB), $0
    MOVW $5, R8
    BNE R8, bnez_true_yes
    MOVW $0, R8
    RET
bnez_true_yes:
    MOVW $1, R8
    RET

TEXT bnez_false(SB), $0
    MOVW $0, R8
    BNE R8, bnez_false_yes
    MOVW $0, R8
    RET
bnez_false_yes:
    MOVW $1, R8
    RET

TEXT bltz_true(SB), $0
    MOVW $-3, R8
    BLT R8, bltz_true_yes
    MOVW $0, R8
    RET
bltz_true_yes:
    MOVW $1, R8
    RET

TEXT bltz_false(SB), $0
    MOVW $3, R8
    BLT R8, bltz_false_yes
    MOVW $0, R8
    RET
bltz_false_yes:
    MOVW $1, R8
    RET

TEXT bgez_true(SB), $0
    MOVW $3, R8
    BGE R8, bgez_true_yes
    MOVW $0, R8
    RET
bgez_true_yes:
    MOVW $1, R8
    RET

TEXT bgez_false(SB), $0
    MOVW $-3, R8
    BGE R8, bgez_false_yes
    MOVW $0, R8
    RET
bgez_false_yes:
    MOVW $1, R8
    RET

// ---- Loop-structure tests ----
// These target the SECOND, still-not-fully-explained part of the
// original bug: an unconditional JMP back to a label preceded by a
// conditional branch compiled into a different conditional branch
// instead. loop_topguard_count reproduces the exact shape the buggy
// l.s used (top guard test, body, unconditional JMP back) with small,
// easy-to-verify-by-hand bounds. Its "expected" value in main.c is
// deliberately set to match what we now BELIEVE happens (0, not 5) -
// this is a regression check confirming our understanding, not a
// hoped-for correct answer. loop_bottomtest_count is the pattern l.s
// was fixed to use (single trailing branch, no leading guard), with
// different bounds than l.s used, to confirm it generalizes rather than
// having only been checked once by luck.

TEXT loop_topguard_count(SB), $0
    MOVW $0, R8
    MOVW $5, R9
topguard_loop:
    BGE R8, R9, topguard_done
    ADD $1, R8
    JMP topguard_loop
topguard_done:
    RET

TEXT loop_bottomtest_count(SB), $0
    MOVW $0, R8
    MOVW $7, R9
bottomtest_loop:
    ADD $1, R8
    BLT R8, R9, bottomtest_loop
    RET
