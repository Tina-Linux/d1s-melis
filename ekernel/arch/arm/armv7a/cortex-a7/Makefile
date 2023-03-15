asflags-y += -I$(srctree)/include/melis/arch/cortex-v7a
asflags-y += -I$(srctree)/include/melis/ekernel/rtos

ccflags-y += -I$(srctree)/include/melis/ekernel/drivers
ccflags-y += -I$(srctree)/ekernel/arch/arm/common $(asflags-y)

CFLAGS_mmu.o := -fno-builtin

ifeq ($(CONFIG_RTTKERNEL), y)
CFLAGS_vector_c.o := -I$(srctree)/ekernel/core/rt-thread/include
endif 

efex:
	$(Q)$(MAKE) $(build)=ekernel/arch/boot/fex

#force build blobdata.S .FORCE will always be create so blobdata would re-create
#every time.
$(obj)/blobdata.o: efex .FORCE

.FORCE:

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
obj-y += blobdata.o
obj-y += tlbinit.o
obj-y += arm_fpu.o
obj-y += tlbop.o
obj-y += port.o
obj-y += swap.o
obj-y += clocksource.o
obj-$(CONFIG_PRINT_CPU_INFO) += cpu_info.o
obj-$(CONFIG_DEBUG_BACKTRACE) += backtrace.o

ifeq ($(CONFIG_KASAN), y)
ccflags-y += -I$(srctree)/ekernel/components/aw/kasan/

obj-${CONFIG_KASAN} += kasan_init.o

KASAN_SANITIZE_kasan_init.o := n
KASAN_SANITIZE_mmu.o := n
KASAN_SANITIZE_vector_c.o := n
endif
