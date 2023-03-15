subdir-ccflags-y +=     -I$(srctree)/include/melis/kernel  \
                        -I$(srctree)/include/melis/module  \
                        -I$(srctree)/include/melis/common  \
                        -I$(srctree)/include/melis/kernel/drivers \
                        -I$(srctree)/include/melis/kernel/pthread \
                        -I$(srctree)/ekernel/components/thirdparty/mmap
obj-y += epos.o
obj-y += ksrv.o
obj-$(CONFIG_SYSCALL) += swi.o
obj-$(CONFIG_SYSCALL) += pthread_swi.o
