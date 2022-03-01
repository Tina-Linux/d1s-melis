#obj-y      += test_app/

#obj-y                           +=  elibs/
#obj-y                           +=  init/
#obj-y                           +=  mod_desktop/
#obj-${CONFIG_SOC_SUN20IW1}      +=  sun20iw1_app/

#include     $(MELIS_BASE)/scripts/Makefile.common
obj-y                           +=  elibs/
always := sub-dirs
PHONY  += sub-dirs
$(obj)/sub-dirs:
	$(Q)$(MAKE) $(build)=$(obj)/sun20iw1_app/res
	$(Q)$(MAKE) $(build)=$(obj)/sun20iw1_app
	

obj-y                           +=  init/
obj-y                           +=  init_sd_product/
obj-y                           +=  mod_desktop/

include     $(MELIS_BASE)/scripts/Makefile.common

