subdir-ccflags-y += -DCONF_NEW_BDMV_STREAM \
    -DCONF_H265_4K \
    -DCONF_KERNEL_VER=310 \
    -DCONF_GPU_MALI \
    -DCONF_KERN_BITWIDE=32 \
    -DCONF_VE_PHY_OFFSET=0x40000000 \
    -D__OS_LINUX

obj-y += \
    cedarx_demux.o \
    aw_demux.o \
    DemuxTypeMap.o

CFLAGS_REMOVE_aw_demux.o += -Werror=implicit-int \
			-Werror=strict-prototypes \
			-Wno-declaration-after-statement \
			-Wno-old-style-declaration \
			-Wno-strict-prototypes \
			-Wno-discarded-qualifiers
