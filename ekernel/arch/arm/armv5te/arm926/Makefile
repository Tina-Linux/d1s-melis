asflags-y += 	-I$(srctree)/include/melis/arch/arm926 \
		-I$(srctree)/include/melis/ekernel/rtos

ccflags-y += 	-I$(srctree)/include/melis/ekernel/drivers \
	     	-I$(srctree)/ekernel/arch/arm/startup $(asflags-y)

CFLAGS_mmu.o := -fno-builtin

ifeq ($(CONFIG_RTTKERNEL), y)
CFLAGS_vector_c.o := -I$(srctree)/ekernel/core/rt-thread/include
endif 

obj-y += vector_s.o
obj-y += vector_c.o
obj-y += port_c.o
obj-y += port_s.o
obj-y += head_s.o
obj-y += entry.o
obj-y += mmu.o
obj-y += fcontext.o
obj-$(CONFIG_RTTKERNEL) += dump_stack.o
obj-$(CONFIG_RTTKERNEL) += backtrace.o
obj-$(CONFIG_ZEPHYRKERNEL) += swap.o
