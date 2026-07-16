/*
 * Minimal M-mode stub for FEL testing: drops to S-mode and jumps to
 * main.bin already loaded elsewhere in DRAM, so the same restrictions
 * apply as when chainloaded via U-Boot's `go` after OpenSBI (Machine-mode-
 * only CSRs trap, illegal instruction is delegated to S-mode) - without
 * needing a full U-Boot boot cycle to exercise start.S's CSR probe.
 *
 * Usage: load main.bin at its normal link address (0x41000000), load this
 * stub at 0x45000 (the usual FEL SRAM scratch address), then
 * `xfel exec 0x45000`. See the `fel-smode` Makefile target.
 */
.section .text.start
.global _start

#define TARGET_ADDR 0x41000000

_start:
    /* Delegate illegal instruction (cause 2) to S-mode, matching what
     * OpenSBI's generic platform does before jumping to U-Boot. Without
     * this, the trap below would go to M-mode's mtvec instead of the
     * target binary's own stvec handler - BROM leaves no handler there,
     * so it wouldn't reproduce the real failure/recovery path. */
    li      t0, (1 << 2)
    csrs    medeleg, t0

    /* mstatus.MPP = S (01), so mret below drops to S-mode instead of
     * returning to M-mode. */
    li      t0, (3 << 11)
    csrc    mstatus, t0
    li      t0, (1 << 11)
    csrs    mstatus, t0

    /* mepc = main.bin's entry point. */
    li      t0, TARGET_ADDR
    csrw    mepc, t0

    mret
