sbi_elf:
	@riscv64-unknown-elf-objcopy -I elf64-littleriscv -O binary fw_jump.elf sbi.bin
	@#riscv64-unknown-elf-objcopy -I binary -O elf64-littleriscv -B riscv sbi.bin sbi.o
	@##rm -fr sbi.bin
clean:
	@rm -fr sbin.bin sbi.o
