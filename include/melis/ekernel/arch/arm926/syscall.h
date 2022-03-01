/*
 * =====================================================================================
 *
 *       Filename:  syscall.h
 *
 *    Description:  syscall layer for user mode, porting from musle libc.
 *
 *        Version:  Melis3.0
 *         Create:  2017-11-02 10:16:48
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2019-10-22 09:45:46
 *
 * =====================================================================================
 */

#ifndef __MELIS_SYSCALL__
#define __MELIS_SYSCALL__

//#define FULL_STACK_SUPPORT
#define DEBUG_SYSCALL

#define __SYSCALL_LL_E(x) \
    ((union { long long ll; long l[2]; }){ .ll = x }).l[0], \
    ((union { long long ll; long l[2]; }){ .ll = x }).l[1]
#define __SYSCALL_LL_O(x) 0, __SYSCALL_LL_E((x))

/*
 *  __syscall : expect the paramter number less than 4,
 *              use r0-r3 reg. pass parameters follow
 *              arm APCS standard.
 *  __syscall5: with 5 parameters syscall routine.
 *  __syscall6: with 6 parameters syscall routine.
 */
extern long __syscallx(long, ...);  /* [> pass para. with r0-r3 <] */
extern long __syscall5(long, long, long, long, long, long);
extern long __syscall6(long, long, long, long, long, long, long);

/*
 * syscall imme. 0xffffff is flags for compat. with kernel compiled by armcc.
 * this is our best attempt to make the kernel and plugins compatable with
 * compilers. the goal is: modules or kernel can be run without consider the
 * compilers. becarefull the corner state of SPSR_svc AND LR_svc is covered.
 */
#ifdef FULL_STACK_SUPPORT
#define __asm_syscall(...) do { \
        __asm__ __volatile__ (  \
                                "svc 0xffffff"              \
                                : "=r"(r0) : __VA_ARGS__ :  \
                                "memory", "lr", "cc");      \
        return r0;              \
    } while (0)
#else
#ifdef DEBUG_SYSCALL
#define __asm_syscall(...) do { \
        __asm__ __volatile__ (  \
                                "mrs   r8,  spsr \n\r"      \
                                "push  {r8}      \n\r"      \
                                "svc   0xffffff  \n\r"      \
                                "pop   {lr}      \n\r"      \
                                "cmp   r8,  lr   \n\r"      \
                                "beq   1f        \n\r"      \
                                "b     .         \n\r"      \
                                "1:              \n\r"      \
                                "msr   spsr_cxsf, r8 \n\r"  \
                                : "=r"(r0) : __VA_ARGS__ :  \
                                "memory", "r8", "lr", "cc");\
        return r0;              \
    } while(0)
#else
#define __asm_syscall(...) do { \
        __asm__ __volatile__ (  \
                                "mrs   r8,  spsr \n\r"      \
                                "svc   0xffffff  \n\r"      \
                                "msr   spsr_cxsf, r8 \n\r"  \
                                : "=r"(r0) : __VA_ARGS__ :  \
                                "memory", "r8", "lr", "cc");\
        return r0;              \
    } while(0)
#endif
#endif

#define __ASM____R7__ __asm__("r7")
#define R7_OPERAND    "r"(r7)

static inline long __syscall0(long n)
{
    register long r7 __ASM____R7__ = n;
    register long r0 __asm__("r0");
    __asm_syscall(R7_OPERAND);
}

static inline long __syscall1(long n, long a)
{
    register long r7 __ASM____R7__ = n;
    register long r0 __asm__("r0") = a;
    __asm_syscall(R7_OPERAND, "0"(r0));
}

static inline long __syscall2(long n, long a, long b)
{
    register long r7 __ASM____R7__ = n;
    register long r0 __asm__("r0") = a;
    register long r1 __asm__("r1") = b;
    __asm_syscall(R7_OPERAND, "0"(r0), "r"(r1));
}

static inline long __syscall3(long n, long a, long b, long c)
{
    register long r7 __ASM____R7__ = n;
    register long r0 __asm__("r0") = a;
    register long r1 __asm__("r1") = b;
    register long r2 __asm__("r2") = c;
    __asm_syscall(R7_OPERAND, "0"(r0), "r"(r1), "r"(r2));
}

static inline long __syscall4(long n, long a, long b, long c, long d)
{
    register long r7 __ASM____R7__ = n;
    register long r0 __asm__("r0") = a;
    register long r1 __asm__("r1") = b;
    register long r2 __asm__("r2") = c;
    register long r3 __asm__("r3") = d;
    __asm_syscall(R7_OPERAND, "0"(r0), "r"(r1), "r"(r2), "r"(r3));
}

#if 0
static inline long __syscall5(long n, long a, long b, long c, long d, long e)
{
    register long r7 __ASM____R7__ = n;
    register long r0 __asm__("r0") = a;
    register long r1 __asm__("r1") = b;
    register long r2 __asm__("r2") = c;
    register long r3 __asm__("r3") = d;
    register long r4 __asm__("r4") = e;
    __asm_syscall(R7_OPERAND, "0"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4));
}

static inline long __syscall6(long n, long a, long b, long c, long d, long e, long f)
{
    register long r7 __ASM____R7__ = n;
    register long r0 __asm__("r0") = a;
    register long r1 __asm__("r1") = b;
    register long r2 __asm__("r2") = c;
    register long r3 __asm__("r3") = d;
    register long r4 __asm__("r4") = e;
    register long r5 __asm__("r5") = f;
    __asm_syscall(R7_OPERAND, "0"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5));
}
#endif
#endif

