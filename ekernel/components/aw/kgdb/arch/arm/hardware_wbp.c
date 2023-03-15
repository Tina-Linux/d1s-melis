#include <stdbool.h>
#include <stdio.h>
#include "hardware_wbp.h"

#define ARM_BASE_BVR    64
#define ARM_BASE_BCR    80
#define ARM_BASE_WVR    96
#define ARM_BASE_WCR    112

#define ARM_OP2_BVR 4
#define ARM_OP2_BCR 5
#define ARM_OP2_WVR 6
#define ARM_OP2_WCR 7

#define ARM_DEBUG_ARCH_V6   1
#define ARM_DEBUG_ARCH_V6_1 2
#define ARM_DEBUG_ARCH_V7_ECP14 3
#define ARM_DEBUG_ARCH_V7_MM    4
#define ARM_DEBUG_ARCH_V7_1 5
#define ARM_DEBUG_ARCH_V8   6

#define ARM_DSCR_HDBGEN (1 << 14) /* DSCR halting bits. */
#define ARM_DSCR_MDBGEN (1 << 15) /* DSCR monitor bits. */

#define  smp_processor_id   cpu_cur_get

#define CPUID_ID    0

/* Accessor macros for the debug registers. */
#define ARM_DBG_READ(N, M, OP2, VAL) do {\
        asm volatile("mrc p14, 0, %0, " #N "," #M ", " #OP2 : "=r" (VAL));\
    } while (0)

#define ARM_DBG_WRITE(N, M, OP2, VAL) do {\
        asm volatile("mcr p14, 0, %0, " #N "," #M ", " #OP2 : : "r" (VAL));\
    } while (0)


#define isb() __asm__ __volatile__ ("" : : : "memory")

#define READ_WB_REG_CASE(OP2, M, VAL)   \
    case ((OP2 << 4) + M):  \
    ARM_DBG_READ(c0, c ## M, OP2, VAL); \
    break

#define WRITE_WB_REG_CASE(OP2, M, VAL)  \
    case ((OP2 << 4) + M):  \
    ARM_DBG_WRITE(c0, c ## M, OP2, VAL);    \
    break

#define GEN_READ_WB_REG_CASES(OP2, VAL) \
    READ_WB_REG_CASE(OP2, 0, VAL);  \
    READ_WB_REG_CASE(OP2, 1, VAL);  \
    READ_WB_REG_CASE(OP2, 2, VAL);  \
    READ_WB_REG_CASE(OP2, 3, VAL);  \
    READ_WB_REG_CASE(OP2, 4, VAL);  \
    READ_WB_REG_CASE(OP2, 5, VAL);  \
    READ_WB_REG_CASE(OP2, 6, VAL);  \
    READ_WB_REG_CASE(OP2, 7, VAL);  \
    READ_WB_REG_CASE(OP2, 8, VAL);  \
    READ_WB_REG_CASE(OP2, 9, VAL);  \
    READ_WB_REG_CASE(OP2, 10, VAL); \
    READ_WB_REG_CASE(OP2, 11, VAL); \
    READ_WB_REG_CASE(OP2, 12, VAL); \
    READ_WB_REG_CASE(OP2, 13, VAL); \
    READ_WB_REG_CASE(OP2, 14, VAL); \
    READ_WB_REG_CASE(OP2, 15, VAL)

#define GEN_WRITE_WB_REG_CASES(OP2, VAL)    \
    WRITE_WB_REG_CASE(OP2, 0, VAL); \
    WRITE_WB_REG_CASE(OP2, 1, VAL); \
    WRITE_WB_REG_CASE(OP2, 2, VAL); \
    WRITE_WB_REG_CASE(OP2, 3, VAL); \
    WRITE_WB_REG_CASE(OP2, 4, VAL); \
    WRITE_WB_REG_CASE(OP2, 5, VAL); \
    WRITE_WB_REG_CASE(OP2, 6, VAL); \
    WRITE_WB_REG_CASE(OP2, 7, VAL); \
    WRITE_WB_REG_CASE(OP2, 8, VAL); \
    WRITE_WB_REG_CASE(OP2, 9, VAL); \
    WRITE_WB_REG_CASE(OP2, 10, VAL);    \
    WRITE_WB_REG_CASE(OP2, 11, VAL);    \
    WRITE_WB_REG_CASE(OP2, 12, VAL);    \
    WRITE_WB_REG_CASE(OP2, 13, VAL);    \
    WRITE_WB_REG_CASE(OP2, 14, VAL);    \
    WRITE_WB_REG_CASE(OP2, 15, VAL)

static uint32_t read_wb_reg(int n)
{
    uint32_t val = 0;

    switch (n)
    {
            GEN_READ_WB_REG_CASES(ARM_OP2_BVR, val);
            GEN_READ_WB_REG_CASES(ARM_OP2_BCR, val);
            GEN_READ_WB_REG_CASES(ARM_OP2_WVR, val);
            GEN_READ_WB_REG_CASES(ARM_OP2_WCR, val);
        default:
            printf("attempt to read from unknown breakpoint register %d\n", n);
    }

    return val;
}

static void write_wb_reg(int n, uint32_t val)
{
    switch (n)
    {
            GEN_WRITE_WB_REG_CASES(ARM_OP2_BVR, val);
            GEN_WRITE_WB_REG_CASES(ARM_OP2_BCR, val);
            GEN_WRITE_WB_REG_CASES(ARM_OP2_WVR, val);
            GEN_WRITE_WB_REG_CASES(ARM_OP2_WCR, val);
        default:
            printf("attempt to write to unknown breakpoint register %d\n", n);
    }
    isb();
}


int monitor_mode_enabled(void)
{
    uint32_t dscr;
    ARM_DBG_READ(c0, c1, 0, dscr);
    return !!(dscr & ARM_DSCR_MDBGEN);
}

#define read_cpuid() \
    ({  \
        unsigned int __val; \
        asm("mrc    p15, 0, %0, c0, c0, 0" \
            : "=r" (__val)  \
            :   \
            : "cc");    \
        __val;  \
    })

static inline unsigned int read_cpuid_id(void)
{
    return read_cpuid();
}

static char get_debug_arch(void)
{
    uint32_t didr;

    /* Do we implement the extended CPUID interface? */
    if (((read_cpuid_id() >> 16) & 0xf) != 0xf)
    {
        printf("CPUID feature registers not supported. "
               "Assuming v6 debug is present.\n");
        return ARM_DEBUG_ARCH_V6;
    }

    ARM_DBG_READ(c0, c0, 0, didr);
    return (didr >> 16) & 0xf;
}
#define CORESIGHT_UNLOCK    0xc5acce55
/* OSLSR os lock model bits */
#define ARM_OSLSR_OSLM0 (1 << 0)

static bool core_has_os_save_restore(void)
{
    uint32_t oslsr;

    switch (get_debug_arch())
    {
        case ARM_DEBUG_ARCH_V7_1:
            return true;
        case ARM_DEBUG_ARCH_V7_ECP14:
            ARM_DBG_READ(c1, c1, 4, oslsr);
            if (oslsr & ARM_OSLSR_OSLM0)
            {
                return true;
            }
        /* Else, fall through */
        default:
            return false;
    }
}

int get_num_brp_resources(void)
{
    uint32_t didr;
    ARM_DBG_READ(c0, c0, 0, didr);
    return ((didr >> 24) & 0xf) + 1;
}

int get_num_wrp_resources(void)
{
    uint32_t didr;
    ARM_DBG_READ(c0, c0, 0, didr);
    return ((didr >> 28) & 0xf) + 1;
}

static int get_num_wrps(void)
{
    if (get_debug_arch() < ARM_DEBUG_ARCH_V7_1)
    {
        return 1;
    }

    return get_num_wrp_resources();
}

static void reset_ctrl_regs(unsigned int cpu)
{
    int i, raw_num_brps, err = 0;
    uint32_t val;
    char debug_arch = get_debug_arch();
    bool has_ossr;
    has_ossr = core_has_os_save_restore();
    int core_num_wrps = get_num_wrps();

    /*
    * v7 debug contains save and restore registers so that debug state
    * can be maintained across low-power modes without leaving the debug
    * logic powered up. It is IMPLEMENTATION DEFINED whether we can access
    * the debug registers out of reset, so we must unlock the OS Lock
    * Access Register to avoid taking undefined instruction exceptions
    * later on.
    */
    switch (debug_arch)
    {
        case ARM_DEBUG_ARCH_V6:
        case ARM_DEBUG_ARCH_V6_1:
            /* ARMv6 cores clear the registers out of reset. */
            goto out_mdbgen;
        case ARM_DEBUG_ARCH_V7_ECP14:
            /*
            * Ensure sticky power-down is clear (i.e. debug logic is
            * powered up).
            */
            ARM_DBG_READ(c1, c5, 4, val);
            if ((val & 0x1) == 0)
            {
                err = -1;
            }

            if (!has_ossr)
            {
                goto clear_vcr;
            }
            break;
        case ARM_DEBUG_ARCH_V7_1:
            /*
            * Ensure the OS double lock is clear.
            */
            ARM_DBG_READ(c1, c3, 4, val);
            if ((val & 0x1) == 1)
            {
                err = -1;
            }
            break;
    }

    if (err)
    {
        printf("CPU %d debug is powered down!\n", cpu);
        return;
    }

    /*
    * Unconditionally clear the OS lock by writing a value
    * other than CS_LAR_KEY to the access register.
    */
    ARM_DBG_WRITE(c1, c0, 4, ~CORESIGHT_UNLOCK);
    isb();

    /*
    * Clear any configured vector-catch events before
    * enabling monitor mode.
    */
clear_vcr:
    ARM_DBG_WRITE(c0, c7, 0, 0);
    isb();

    /*
    * The control/value register pairs are UNKNOWN out of reset so
    * clear them to avoid spurious debug events.
    */
    raw_num_brps = get_num_brp_resources();
    for (i = 0; i < raw_num_brps; ++i)
    {
        write_wb_reg(ARM_BASE_BCR + i, 0UL);
        write_wb_reg(ARM_BASE_BVR + i, 0UL);
    }

    for (i = 0; i < core_num_wrps; ++i)
    {
        write_wb_reg(ARM_BASE_WCR + i, 0UL);
        write_wb_reg(ARM_BASE_WVR + i, 0UL);
    }

    /*
    * Have a crack at enabling monitor mode. We don't actually need
    * it yet, but reporting an error early is useful if it fails.
    */
out_mdbgen:
    return;
}

static int cpu_cur_get(void)
{
    return 0;
}

int enable_monitor_mode(void)
{
    uint32_t dscr;
    uint32_t val;

    reset_ctrl_regs(cpu_cur_get());

    ARM_DBG_READ(c0, c1, 0, dscr);

    /* If monitor mode is already enabled, just return. */
    if (dscr & ARM_DSCR_MDBGEN)
    {
        goto out;
    }

    /* Write to the corresponding DSCR. */
    switch (get_debug_arch())
    {
        case ARM_DEBUG_ARCH_V6:
        case ARM_DEBUG_ARCH_V6_1:
            ARM_DBG_WRITE(c0, c1, 0, (dscr | ARM_DSCR_MDBGEN));
            break;
        case ARM_DEBUG_ARCH_V7_ECP14:
        case ARM_DEBUG_ARCH_V7_1:
        case ARM_DEBUG_ARCH_V8:
            ARM_DBG_WRITE(c0, c2, 2, (dscr | ARM_DSCR_MDBGEN));
            isb();
            break;
        default:
            return -1;
    }

    /* Check that the write made it through. */
    ARM_DBG_READ(c0, c1, 0, dscr);
    if (!(dscr & ARM_DSCR_MDBGEN))
    {
        printf("Failed to enable monitor mode on CPU %d.\n",
               smp_processor_id());
        return -1;
    }

out:
    return 0;
}

/*
 * @param type: watchpoint type
 * @param i   : watchpoint id, range from 0~3
 * @param addr: virtual memory address
 * set a hardware watchpoint, it must be running in debug monitor mode.
 */
int arm_install_hw_watchpoint(enum gdb_bptype type, int i, uint32_t addr)
{
    uint32_t ctrl = 0x1F7;
    uint32_t check_value;

    switch (type)
    {
        case BP_WRITE_WATCHPOINT:
            ctrl = 0x1F7;
            break;
        case BP_READ_WATCHPOINT:
            ctrl = 0x1EF;
            break;
        case BP_ACCESS_WATCHPOINT:
            ctrl = 0x1FF;
            break;
        default:
            return -1;
    }

    addr &= ~(0x4UL - 1);

    /* Setup the address register. */
    write_wb_reg(ARM_BASE_WVR + i, addr);
    check_value = read_wb_reg(ARM_BASE_WVR + i);
    if (check_value != addr)
    {
        printf("fail to set WVR[%d] addr:0x%lx check_value:0x%lx\n", i, addr, check_value);
        return -1;
    }

    /* Setup the control register. */
    write_wb_reg(ARM_BASE_WCR + i, ctrl);
    check_value = read_wb_reg(ARM_BASE_WCR + i);
    if (check_value != ctrl)
    {
        printf("fail to set WCR[%d] ctrl:0x%lx check_value:0x%lx\n", i, ctrl, check_value);
        return -1;
    }

    return 0;
}

/*
 * @param i   : watchpoint id, range from 0~5
 * remove a hardware watchpoint, it must be running in debug monitor mode.
 */
void arm_uninstall_hw_watchpoint(int i)
{
    write_wb_reg(ARM_BASE_WCR + i, 0UL);
    write_wb_reg(ARM_BASE_WVR + i, 0UL);
}

/*
 * @param type: breakpoint type
 * @param i   : breakpoint id, range from 0~5
 * @param addr: virtual memory address
 * set a hardware breakpoint, it must be running in debug monitor mode.
 */
int arm_install_hw_breakpoint(int i, uint32_t addr)
{
    uint32_t ctrl = 0x1E7;//0x1E7;
    uint32_t check_value;

    addr &= ~(0x4UL - 1);

    /* Setup the address register. */
    write_wb_reg(ARM_BASE_BVR + i, addr);
    check_value = read_wb_reg(ARM_BASE_BVR + i);
    if (check_value != addr)
    {
        printf("fail to set BVR[%d] addr:0x%lx check_value:0x%lx\n", i, addr, check_value);
        return -1;
    }

    /* Setup the control register. */
    write_wb_reg(ARM_BASE_BCR + i, ctrl);
    check_value = read_wb_reg(ARM_BASE_BCR + i);
    if (check_value != ctrl)
    {
        printf("fail to set BCR[%d] ctrl:0x%lx check_value:0x%lx\n", i, ctrl, check_value);
        return -1;
    }

    return 0;
}

/*
 * @param i   : watchpoint id, range from 0~5
 * remove a hardware breakpoint, it must be running in debug monitor mode.
 */
void arm_uninstall_hw_breakpoint(int i)
{
    write_wb_reg(ARM_BASE_BCR + i, 0UL);
    write_wb_reg(ARM_BASE_BVR + i, 0UL);
}
