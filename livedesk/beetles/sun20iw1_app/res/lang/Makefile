include $(MELIS_BASE)/scripts/Makefile.common

always := next-dirs

NOW_PATH    =   $(MELIS_BASE)/${SOLUTION}/beetles/sun20iw1_app/res/lang
HEAD_PATH   =   $(MELIS_BASE)/${SOLUTION}/beetles/include/res
BIN_PATH    =   $(srctree)/projects/${TARGET_BOARD}/data/UDISK/apps/

PHONY += next-dirs clean
$(obj)/next-dirs:
	@/bin/bash $(NOW_PATH)/makelang.sh $(MELIS_BASE)

__clean : clean

clean:
	@-rm -rf $(NOW_PATH)/lang.h $(NOW_PATH)/lang.bin $(HEAD_PATH)/lang.h $(BIN_PATH)/lang.bin
