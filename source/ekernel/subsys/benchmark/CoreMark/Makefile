PORT_CFLAGS = -O2
FLAGS_STR = "$(PORT_CFLAGS) $(XCFLAGS) $(XLFLAGS) $(LFLAGS_END)"
ccflags-y = $(PORT_CFLAGS) -I$(PORT_DIR) -I. -DFLAGS_STR=\"$(FLAGS_STR)\" -DMEM_METHOD=0

subdir-ccflags-y :=     -I$(srctree)/include/melis/kernel \
    			-I$(obj)/arm \
    			-I$(obj) \

obj-y +=  core_list_join.o
obj-y +=  core_main.o
obj-y +=  core_state.o
obj-y +=  core_util.o
obj-y +=  core_matrix.o
obj-y +=  arm/core_portme.o
