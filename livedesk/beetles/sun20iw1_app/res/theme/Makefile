include $(MELIS_BASE)/scripts/Makefile.common

always := next-dirs

NOW_PATH    =   $(MELIS_BASE)/${SOLUTION}/beetles/sun20iw1_app/res/theme
HEAD_PATH   =   $(MELIS_BASE)/${SOLUTION}/beetles/include/res
BIN_PATH    =   $(srctree)/projects/${TARGET_BOARD}/data/UDISK/apps/


PHONY += next-dirs clean
$(obj)/next-dirs:
	@/bin/bash $(NOW_PATH)/maketheme.sh $(MELIS_BASE)

__clean : clean

clean:
	@-rm -rf $(NOW_PATH)/theme.h $(NOW_PATH)/theme.bin $(HEAD_PATH)/theme.h $(BIN_PATH)/theme.bin 
