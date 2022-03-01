subdir-ccflags-y += \
    -I$(srctree)/ekernel/subsys/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libcedarx/libcore/parser/aac \
    -I$(srctree)/ekernel/subsys/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libcedarx/libcore/parser/id3v2 \
    -I$(srctree)/ekernel/subsys/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libcedarx/libcore/parser/base/id3base \
    -I$(srctree)/ekernel/subsys/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libcedarx/libcore/parser/ts

subdir-ccflags-y += -DCONF_NEW_BDMV_STREAM \
    -DCONF_H265_4K \
    -DCONF_KERNEL_VER=310 \
    -DCONF_GPU_MALI \
    -DCONF_KERN_BITWIDE=32 \
    -DCONF_VE_PHY_OFFSET=0x40000000 \
    -D__OS_LINUX

obj-y += libcore/base/
obj-y += libcore/common/
obj-$(CONFIG_mpp_demuxer) += \
    libcore/stream/file/CdxFileStream.o \
    libcore/stream/base/CdxStream.o \
    libcore/parser/aac/CdxAacParser.o \
    libcore/parser/id3v2/CdxId3v2Parser.o \
    libcore/parser/mp3/CdxMp3Parser.o \
    libcore/parser/mov/CdxMovAtom.o \
    libcore/parser/mov/CdxMovList.o \
    libcore/parser/mov/CdxMovParser.o \
    libcore/parser/mov/CdxMovParserImpl.o \
    libcore/parser/mov/CdxMovSample.o \
    libcore/parser/mov/mpeg4Vol.o \
    libcore/parser/ts/CdxTsParser.o \
    libcore/parser/ts/MediaProbe.o \
    libcore/parser/base/CdxParser.o \
    libcore/parser/base/VideoSpecificData.o \
    libcore/parser/base/id3base/StringContainer.o \
    libcore/parser/base/id3base/Id3Base.o \
    libcore/parser/base/id3base/CdxUtfCode.o \
    libcore/parser/base/id3base/CdxMetaData.o

#[$(obj)]:[ekernel/subsys/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libcedarx]
#$(MELIS_BASE)]:[/home/ericwang/workspace/exdroid/melis-3.x-v459/source]
$(obj)/copyfiles:
	@echo "  COPY    v316_cedarx.conf---> cedarx.conf"
	@(mkdir -p $(MELIS_BASE)/workspace/suniv/rootfs/etc; \
		cp $(obj)/config/v316_cedarx.conf $(MELIS_BASE)/workspace/suniv/rootfs/etc/cedarx.conf)

always += copyfiles

