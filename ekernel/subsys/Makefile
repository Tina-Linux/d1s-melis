subdir-ccflags-y += -I$(srctree)/ekernel/filesystem/include
subdir-ccflags-y += -I$(srctree)/ekernel/core/rt-thread/include
subdir-ccflags-y += -I$(srctree)/ekernel/subsys
subdir-ccflags-y += -I$(srctree)/ekernel/subsys/thirdparty/dfs/include
subdir-ccflags-y += -I$(srctree)/ekernel/subsys/finsh_cli
subdir-ccflags-y += -I$(srctree)/ekernel/drivers/include/osal
subdir-ccflags-y += -I$(srctree)/ekernel/drivers/include/hal
subdir-ccflags-y += -I$(srctree)/ekernel/drivers/include/drv

obj-${CONFIG_PTHREAD} += pthreads/
obj-${CONFIG_EXTERN_LIBC} += libc/
obj-${CONFIG_BENCHMARK} += benchmark/
obj-${CONFIG_CXX} += cplusplus/
obj-${CONFIG_SAMPLES} += samples/
obj-y += samples/
obj-y += lib/
obj-${CONFIG_RT_USING_FINSH} += finsh_cli/
obj-y += debug/
obj-${CONFIG_NETWORK} += net/
obj-y += aw/
obj-y += thirdparty/
obj-y += config/
obj-y += env/
obj-y += avframework/
obj-y += standby/
obj-${CONFIG_SUBSYS_ARCHIVAL} += archival/

ifneq ($(CONFIG_SDK_RELEASE),y)
obj-${CONFIG_COMPONENT_ADBD} += adbd/
endif
obj-${CONFIG_COMPONENT_MTP} += mtp/
