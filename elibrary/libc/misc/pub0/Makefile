subdir-ccflags-y +=-I. -I../

#lib-y += cacheop/cleanflushcachebyregion.o
lib-${CONFIG_ARM} += critical/critical.o
lib-${CONFIG_RISCV} += critical/critical_rv.o
#lib-y += usb_ops/usb_cd.o
#lib-y += usb_ops/usb_plug.o
lib-y += reg/elibs_reg.o

TARGET := $(srctree)/${elibrary-libs}/libpub0.a

include $(MELIS_BASE)/scripts/Makefile.rename
