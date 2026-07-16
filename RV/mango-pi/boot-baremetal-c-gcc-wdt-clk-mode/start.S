.section .text.start
.global _start

/*
 * T-Head C906 custom CSRs (not standard RISC-V, so no symbolic names in
 * the assembler - use raw numbers). These control I-cache/D-cache
 * enable and branch/prefetch hints. Values and CSR numbers are exactly
 * what U-Boot's SPL sets in board/sunxi/board-riscv.c:spl_board_init_f()
 * before jumping to U-Boot proper. Without them (e.g. booted directly via
 * FEL/xfel, which skips SPL entirely and inherits the BROM's much more
 * conservative reset defaults) the blink loop below runs about 10x
 * slower, so we set them ourselves to get consistent performance
 * regardless of boot path.
 *
 * These four are Machine-mode-only CSRs. Writing them from S-mode raises
 * an illegal instruction exception - the CSR address itself is fine, it's
 * just inaccessible below M-mode. Which mode we're actually in depends on
 * how we got here:
 *   - Direct FEL boot (`xfel exec`): BROM never leaves M-mode, so we
 *     inherit M-mode and the writes succeed normally.
 *   - Chainloaded via U-Boot (`go`): OpenSBI already dropped privilege to
 *     S-mode with an mret before ever jumping to U-Boot, and U-Boot's
 *     `go` is a bare jump with no privilege transition - so we run in
 *     S-mode, and these writes would trap.
 * There's no instruction to ask "what mode am I in" or to raise privilege
 * back to M-mode on demand, so instead of maintaining two builds we probe:
 * install our own trap handler (csr_probe_trap) around the writes. If we
 * are in M-mode nothing traps and the handler is never used. If we are in
 * S-mode each write traps, and the handler just skips the single 4-byte
 * faulting instruction (sepc += 4) and resumes - a correct no-op, since
 * U-Boot's SPL already did this setup in M-mode before we ever got
 * control. This relies on OpenSBI delegating illegal-instruction
 * exceptions to S-mode (medeleg), which is the generic-platform default
 * and is why U-Boot's own exception handler was the one printing "illegal
 * instruction" before this handler was added.
 */
#define CSR_MXSTATUS 0x7c0
#define CSR_MHCR     0x7c1
#define CSR_MCOR     0x7c2
#define CSR_MHINT    0x7c5

_start:
    /* t1 holds the previous stvec across the whole probe block - the
     * trap handler must not clobber it, so it uses t2 as its own scratch
     * instead. */
    csrr    t1, stvec
    la      t2, csr_probe_trap
    csrw    stvec, t2

    li      t0, 0x638000
    csrs    CSR_MXSTATUS, t0

    li      t0, 0x70013
    csrw    CSR_MCOR, t0

    li      t0, 0x11ff
    csrw    CSR_MHCR, t0

    li      t0, 0x16e30c
    csrw    CSR_MHINT, t0

    csrw    stvec, t1

    la      sp, _stack_top

    /* zero .bss */
    la      t0, _bss_start
    la      t1, _bss_end
1:
    bge     t0, t1, 2f
    sw      zero, 0(t0)
    addi    t0, t0, 4
    j       1b
2:

    call    main

hang:
    j       hang

/*
 * Used only while probing the Machine-mode-only CSRs above. On illegal
 * instruction (the only fault expected here), skip the single 4-byte
 * instruction that faulted and resume. Runs before sp is set up, so it
 * must stay register-only - no stack available.
 */
.align 2
csr_probe_trap:
    csrr    t2, sepc
    addi    t2, t2, 4
    csrw    sepc, t2
    sret
