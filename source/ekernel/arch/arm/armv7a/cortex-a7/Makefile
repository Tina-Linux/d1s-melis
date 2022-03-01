asflags-y += 	-I$(srctree)/include/melis/arch/cortex-v7a \
		-I$(srctree)/include/melis/ekernel/rtos

ccflags-y += 	-I$(srctree)/include/melis/ekernel/drivers \
		-I$(srctree)/ekernel/arch/arm/startup $(asflags-y)

CFLAGS_mmu.o := -fno-builtin

ifeq ($(CONFIG_RTTKERNEL), y)
CFLAGS_vector_c.o := -I$(srctree)/ekernel/core/rt-thread/include
endif 

obj-y += vector_s.o
obj-y += cacheop.o
obj-y += vector_c.o
obj-y += port_c.o
obj-y += port_s.o
obj-y += head_s.o
obj-y += entry.o
obj-y += mmu.o
obj-y += fcontext.o
obj-y += page_2nd_level.o
obj-y += arm_dump.o
obj-$(CONFIG_DEBUG_BACKTRACE) += debug/backtrace.o
obj-y += blobdata.o
obj-y += tlbinit.o
obj-y += arm_fpu.o
obj-y += cache_info.o
obj-y += tlbop.o
obj-y += delay.o
obj-y += port.o
obj-y += show_pte.o
obj-y += swap.o
obj-y += clocksource.o
obj-y += spinlock/spin_lock.o
obj-$(CONFIG_RTTKERNEL) += dump_stack.o
obj-$(CONFIG_RTTKERNEL) += backtrace.o
