subdir-ccflags-y += -I$(srctree)/ekernel/core/rt-thread/include
subdir-ccflags-y += -I$(srctree)/ekernel/components/thirdparty/finsh_cli

lib-y += melis/syscall.o
lib-y += melis/syscall_fops.o
lib-${CONFIG_NETWORK} += melis/syscall_lwip.o
lib-${CONFIG_ARMV7_A} += arch/arm/armv7a/syscall.o
lib-${CONFIG_ARMV5TE} += arch/arm/armv5te/syscall.o
lib-${CONFIG_PTHREAD} += pthread/syscall.o

TARGET := $(srctree)/${elibrary-libs}/libsyscall.a

include $(MELIS_BASE)/scripts/Makefile.rename
