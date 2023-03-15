subdir-ccflags-y += -DHAVE_AV_CONFIG_H
subdir-ccflags-y += -Wno-attributes
subdir-ccflags-y += \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libmuxer/include \
    -I$(srctree)/ekernel/components/avframework/eyesee-mpp/middleware/sun8iw19p1/media/LIBRARY/libmuxer/common

obj-y += \
    common/libavutil/intmath.o \
    common/libavutil/mem.o \
    mp3_muxer/mp3.o \
    mp3_muxer/Mp3Muxer.o \
    aac_muxer/AacMuxer.o \
    mp4_muxer/avc.o \
    mp4_muxer/ByteIOContext.o \
    mp4_muxer/hevc.o \
    mp4_muxer/Mp4Muxer.o \
    mp4_muxer/Mp4MuxerDrv.o \
    mp4_muxer/sa_config.o \
    mpeg2ts_muxer/Mpeg2tsMuxerDrv.o \
    mpeg2ts_muxer/Mpeg2tsMuxer.o \
    raw_muxer/RawMuxer.o \
    muxers/record_writer.o
