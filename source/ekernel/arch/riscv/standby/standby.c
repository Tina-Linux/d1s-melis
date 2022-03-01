/*
 * riscv64 standby
 */
#include <sbi.h>
#include <init.h>
#include <melis/standby/standby.h>
#include <csr.h>

static unsigned long csr;

static void riscv64_standby_enter(int level)
{
	sbi_standby(level);
	return;
}

static void riscv64_core_suspend(void)
{
	csr = csr_read_clear(sie, (SIE_STIE | SIE_SSIE | SIE_SMIE));

	return;
}

static void riscv64_core_resume(void)
{
	csr_write(sie, csr);
	return;
}

static int riscv64_standby_init(void)
{
	arch_enter_standby = riscv64_standby_enter;
	arch_core_suspend = riscv64_core_suspend;
	arch_core_resume = riscv64_core_resume;

	return 0;
}

arch_initcall(riscv64_standby_init);
