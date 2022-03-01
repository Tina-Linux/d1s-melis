include $(MELIS_BASE)/scripts/Makefile.common

subdir-ccflags-y +=-I. -I../

lib-y += elibs_mediainf.o

always += mergelibs

PHONY += mergelibs 

$(obj)/mergelibs: $(obj)/lib.a FORCE
	$(Q) echo OPEN $(obj)/lib.a > $(obj)/ar.mac
	$(Q) echo ADDLIB $(obj)/3rd/${LIB_DIR}/GetAudio_format.a >> $(obj)/ar.mac
	$(Q) echo SAVE >> $(obj)/ar.mac
	$(Q)$(CROSS_COMPILE)ar -M < $(obj)/ar.mac
	$(Q)mv $(obj)/lib.a $(srctree)/${elibrary-libs}/libmediainfo.a
	$(Q)rm -f $(obj)/ar.mac
	$(Q)rm -f $(obj)/tmp-libmediainfo.a
