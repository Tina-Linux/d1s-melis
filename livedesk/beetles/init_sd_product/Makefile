ccflags-y +=	-I$(MELIS_BASE)/include/melis/kernel/drivers  \
				-I$(MELIS_BASE)/include/melis/kernel \
				-I$(MELIS_BASE)/include/melis \
				-I$(MELIS_BASE)/include/melis/misc   \
				-I$(MELIS_BASE)/include/melis/libs/libc \
				-I$(MELIS_BASE)/include/melis/emodules     \
				-I$(MELIS_BASE)/include/melis/emodules/mod_orange/gui/core     \
				-I$(MELIS_BASE)/livedesk/beetles/include/mod_desktop/functions     \
				-I$(MELIS_BASE)/livedesk/beetles/include/mod_desktop/functions/display     \
				-I$(MELIS_BASE)/livedesk/beetles/include/mod_desktop/functions/theme     \
				-I$(MELIS_BASE)/livedesk/beetles/include/mod_desktop/functions/charset     \
				-I$(MELIS_BASE)/livedesk/beetles/include/res \
				-I$(MELIS_BASE)/ekernel/drivers/include/drv \
				-I$(MELIS_BASE)/livedesk/beetles/include \
				-DEPDK_DEBUG_LEVEL=EPDK_DEBUG_LEVEL_LOG_ALL -DUSED_BY_INIT \

usrlibs-y   +=  -L$(srctree)/${elibrary-libs}/      \
				--start-group                       \
				-lsyscall   -llzma                  \
				-lminic     -lpub0                  \
				-lcharenc   -lmediainfo             \
				-lcharset                           \
				-lelibs_ex  -lapp_views             \
				--end-group
MOD_NAME := init_sd_product
SUF_NAME := axf
PRJ_PATH    := $(MELIS_BASE)/projects/${TARGET_BOARD}/data/UDISK/apps
TEMP_PATH   := $(MELIS_BASE)/emodules/bin


$(MOD_NAME)-objs += init_api.o
$(MOD_NAME)-objs += mod_init.o
$(MOD_NAME)-objs += magic.o

include $(MELIS_BASE)/scripts/Makefile.mods
