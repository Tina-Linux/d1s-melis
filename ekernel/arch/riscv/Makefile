subdir-ccflags-y +=     -I$(srctree)/ekernel/arch/riscv/atomic/include \
                        -I$(srctree)/ekernel/arch/riscv/common \
                        -I$(srctree)/ekernel/arch/riscv/rv64gc/mmu \
                        -I$(srctree)/ekernel/arch/riscv/sbi \
                        -I$(srctree)/ekernel/arch/riscv/include

subdir-asflags-y +=     -I$(srctree)/ekernel/arch/riscv/include
subdir-asflags-y +=     -I$(srctree)/ekernel/arch/riscv/common

ifeq ($(CONFIG_RV64), y)
subdir-asflags-y +=     -I$(srctree)/ekernel/arch/riscv/rv64gc/include
subdir-ccflags-y +=     -I$(srctree)/ekernel/arch/riscv/rv64gc/include
else
subdir-asflags-y +=     -I$(srctree)/ekernel/arch/riscv/rv32/include
subdir-ccflags-y +=     -I$(srctree)/ekernel/arch/riscv/rv32/include
endif

obj-y += common/
obj-y += sunxi/

# chip supporting package driver.
obj-y += csp/

obj-${CONFIG_RV64} += rv64gc/
obj-${CONFIG_RV32} += rv32/
obj-${CONFIG_RV_PLIC} += plic/
ifneq ($(CONFIG_STANDBY_MSGBOX), y)
obj-y += standby/
endif
