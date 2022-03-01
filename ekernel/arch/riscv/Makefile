subdir-ccflags-y +=     -I$(srctree)/ekernel/arch/riscv/atomic/include \
                        -I$(srctree)/ekernel/arch/riscv/common \
                        -I$(srctree)/ekernel/arch/riscv/rv64gc/include \
                        -I$(srctree)/ekernel/arch/riscv/rv64gc/mmu \
                        -I$(srctree)/ekernel/arch/riscv/sbi \

subdir-asflags-y +=     -I$(srctree)/ekernel/arch/riscv/rv64gc/include

obj-y += common/
obj-y += sunxi/

# chip supporting package driver.
obj-y += csp/

obj-y += rv64gc/
obj-y += plic/
obj-y += standby/
