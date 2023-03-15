#include <kgdb.h>
#include <gdb_stub.h>
#include <stdio.h>

extern struct gdb_arch arch_gdb_ops;

int kgdb_permit_data_access(unsigned long addr, int length)
{
    unsigned long end_addr = addr + length;
    if (!arch_gdb_ops.check_data_access)
    {
        return -1;
    }

    if (arch_gdb_ops.check_data_access(addr) != 1)
    {
        return -1;
    }

    if (arch_gdb_ops.check_data_access(end_addr) != 1)
    {
        return -1;
    }

    return 0;
}

void gdb_start(void)
{
    if (arch_gdb_ops.set_compiled_breakpoint)
    {
        arch_gdb_ops.set_compiled_breakpoint();
        return;
    }
    printf("can not set compiled breakpoint!!\n");
}

int kgdb_arch_init(void)
{
    if (arch_gdb_ops.kgdb_init)
    {
        return arch_gdb_ops.kgdb_init();
    }
    return -1;
}

int kgdb_arch_exit(void)
{
    if (arch_gdb_ops.kgdb_exit)
    {
        return arch_gdb_ops.kgdb_exit();
    }
    return -1;
}
