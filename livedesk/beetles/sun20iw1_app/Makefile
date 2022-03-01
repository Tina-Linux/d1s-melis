#obj-y += apps/
#obj-y += res/
#include     $(MELIS_BASE)/scripts/Makefile.common

always := subdirs

PHONY += subdirs 
$(obj)/subdirs:
	$(Q)$(MAKE) $(build)=$(obj)/apps

__clean: clean

clean:
	$(Q)$(MAKE) $(clean)=$(obj)/apps

