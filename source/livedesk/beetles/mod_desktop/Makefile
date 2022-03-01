ccflags-y           +=  -DEPDK_DEBUG_LEVEL=EPDK_DEBUG_LEVEL_LOG_ALL         \
                        -DUSED_BY_DESKTOP                                   \
                        $(SOLUTION_INCLUDE)

usrlibs-y           +=  -L$(srctree)/${elibrary-libs}/          \
                        --start-group                           \
                        -lsyscall    -lminic                    \
                        -llzma       -lcharset                  \
                        -lmediainfo                             \
                        --end-group

#-lmediainfo
MOD_NAME            := desktop
SUF_NAME            := mod
PRJ_PATH            := $(MELIS_BASE)/projects/${TARGET_BOARD}/data/UDISK/apps/
TEMP_PATH           := $(MELIS_BASE)/emodules/bin

$(MOD_NAME)-objs    += desktop_api.o
$(MOD_NAME)-objs    += mod_desktop.o
$(MOD_NAME)-objs    += magic.o
$(MOD_NAME)-objs    += built-in.o

$(MOD_NAME)-objs    += engine/
$(MOD_NAME)-objs    += framework/
$(MOD_NAME)-objs    += functions/
$(MOD_NAME)-objs    += msg_srv/
$(MOD_NAME)-objs    += util/

include $(MELIS_BASE)/scripts/Makefile.mods
