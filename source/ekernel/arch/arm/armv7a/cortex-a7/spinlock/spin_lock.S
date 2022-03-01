#define UNLOCK_STATUS  0xff

.global __init_key
__init_key:
    mov     r1, #UNLOCK_STATUS
    str     r1, [r0]
    bx      lr

.global __lock_key_arm
__lock_key_arm:
    ldrex   r1, [r0]
    cmp     r1, #UNLOCK_STATUS
    wfene
    bne     __lock_key_arm

    mrc     p15, 0, r1, c0, c0, 5
    and     r1, r1, #0x03
    strex   r2, r1, [r0]
    cmp     r2, #0x0
    bne     __lock_key_arm

    dmb
    isb
    dsb

    bx      lr

.global __unlock_key_arm
__unlock_key_arm:
    mrc     p15, 0, r1, c0, c0, 5   @ Read CPU ID register
    and     r1, r1, #0x03           @ Mask off, leaving the CPU ID in r1
    ldr     r2, [r0]                @ Read the lock field of the mutex
    cmp     r1, r2                  @ Compare ID of this CPU with the lock owner
    movne   r0, #0x1                @ If ID doesn't match, return "fail"
    bxne    lr

    dmb                             @ Ensure that accesses to shared resource have completed
    isb
    dsb

    mov     r1, #UNLOCK_STATUS      @ Write "unlocked" into lock field
    str     r1, [r0]

    dsb                             @ Ensure that no instructions following the barrier execute until
    sev                             @ Send event to other CPUs, wakes anyone waiting on a mutex (using WFE)
    dmb

    mov     r0, #0x0                @ Return "success"
    bx      lr

