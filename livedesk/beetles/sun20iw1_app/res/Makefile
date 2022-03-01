always := next-dirs

PHONY += next-dirs clean
$(obj)/next-dirs:
	$(Q)$(MAKE) $(build)=$(obj)/lang
	$(Q)$(MAKE) $(build)=$(obj)/theme

__clean : clean

clean:
	$(Q)$(MAKE) $(clean)=$(obj)/lang
	$(Q)$(MAKE) $(clean)=$(obj)/theme