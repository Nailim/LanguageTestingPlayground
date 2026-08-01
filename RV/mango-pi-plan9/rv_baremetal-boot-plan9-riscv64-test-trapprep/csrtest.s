// CSR write/read roundtrip tests - see README. Only CSR READS
// (MOVW CSR(n), Rd) were previously proven (the D1 diag probe); CSR
// WRITES (MOVW Rd, CSR(n)) are untested until now, and Phase 2's first
// real act is writing stvec to install the trap vector - a silently
// wrong write there means traps never reach the handler at all.
//
// sscratch is the primary target: a CSR with zero side effects by RISC-V
// spec design (general-purpose kernel scratch storage), safe to write
// arbitrary bit patterns repeatedly with no risk to the rest of the
// test. Three distinct patterns per roundtrip: 0 (all-zero), -1
// (all-one, catches width truncation the same way regtest.s's -n
// pattern does), and a small positive value (catches a "always echoes
// the last write" or "always reads back the complement" class of bug
// the 0/-1 pair alone wouldn't distinguish).
//
// stvec/sepc get one lighter roundtrip check each, using small,
// already-4-byte-aligned values - not because a realistic address is
// needed (this only tests the write/read mechanism, not a real trap
// vector), but to sidestep stvec's WARL mode-field bits in the low 2
// bits without needing to predict how this implementation masks them,
// and to avoid the untested territory of loading a large bare numeric
// immediate (as opposed to a $symbol(SB) address, which IS proven).
// Both are restored to 0 before RET, since neither is a real trap
// vector / return address in this test.

TEXT csr_sscratch_zero(SB), $0
    MOVW $0, R8
    MOVW R8, CSR(0x140)
    MOVW CSR(0x140), R8
    RET

TEXT csr_sscratch_allones(SB), $0
    MOVW $-1, R8
    MOVW R8, CSR(0x140)
    MOVW CSR(0x140), R8
    RET

TEXT csr_sscratch_pattern(SB), $0
    MOVW $305, R8
    MOVW R8, CSR(0x140)
    MOVW CSR(0x140), R8
    RET

TEXT csr_stvec_roundtrip(SB), $0
    MOVW $0x100, R8
    MOVW R8, CSR(0x105)
    MOVW CSR(0x105), R8
    MOVW $0, R9
    MOVW R9, CSR(0x105)	// restore to a safe/inert value before RET
    RET

TEXT csr_sepc_roundtrip(SB), $0
    MOVW $0x200, R8
    MOVW R8, CSR(0x141)
    MOVW CSR(0x141), R8
    MOVW $0, R9
    MOVW R9, CSR(0x141)	// restore to a safe/inert value before RET
    RET
