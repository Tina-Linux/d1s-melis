#ifndef HARDWARE_WBP_H
#define HARDWARE_WBP_H

#include <stdint.h>
#include <gdb_stub.h>

int monitor_mode_enabled(void);
int enable_monitor_mode(void);

int get_num_brp_resources(void);
int get_num_wrp_resources(void);

void arm_uninstall_hw_breakpoint(int i);
int arm_install_hw_breakpoint(int i, uint32_t addr);
void arm_uninstall_hw_watchpoint(int i);
int arm_install_hw_watchpoint(enum gdb_bptype type, int i, uint32_t addr);

#endif  /*HARDWARE_WBP_H*/
