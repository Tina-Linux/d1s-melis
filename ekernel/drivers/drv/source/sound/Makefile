subdir-ccflags-y +=	-I$(srctree)/ekernel/drivers/include \
			-I$(srctree)/ekernel/core/rt-thread/include \
			-I$(srctree)/include/melis/audio \
			-I$(srctree)/include/melis/libs/libc \
			-I$(srctree)/ekernel/drivers/include/hal/aw-alsa-lib \
			-I$(srctree)/ekernel/drivers/hal/source/sound/component/aw-alsa-lib/external_resample/speexrate

obj-y += sound_drv.o
obj-y += circle_buf.o
obj-y += drv_audio.o
obj-y += audio_play.o
##obj-y += audio_play_task.o
obj-y += audio_rec.o
obj-y += audio_rec_task.o
