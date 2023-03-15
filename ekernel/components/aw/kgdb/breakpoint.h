#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#include "gdb_stub.h"

#ifndef CONFIG_GDB_MAX_SW_BREAKPOINTS
#define GDB_MAX_SW_BREAKPOINTS 20
#else
#define GDB_MAX_SW_BREAKPOINTS  CONFIG_GDB_MAX_SW_BREAKPOINTS
#endif

int gdb_remove_sw_break(unsigned long addr, int len);
int gdb_isremoved_sw_break(unsigned long addr);
int gdb_set_sw_break(unsigned long addr, int len);

int gdb_set_hw_break(unsigned long addr);
int gdb_remove_hw_break(unsigned long addr);
int gdb_isremoved_hw_break(unsigned long addr);

int gdb_set_hw_watch(unsigned long addr, enum gdb_bptype type);
int gdb_remove_hw_watch(unsigned long addr);
int gdb_isremoved_hw_watch(unsigned long addr);

int remove_all_break_watch_points(void);

int gdb_deactivate_all_break_watch_points(void);
int gdb_activate_all_break_watch_points(void);

int destory_hw_break_watch(void);
int create_hw_break_watch(unsigned int hw_break, unsigned int hw_watch);

void debug_dump_all_breaks_info(void);
#endif  /*BREAKPOINT_H*/
