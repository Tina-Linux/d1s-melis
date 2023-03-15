# Common dummy dai
obj-$(CONFIG_SND_PLATFORM_SUNXI_CPUDAI) += sunxi-dummy-cpudai.o

# Common dmaengine pcm
obj-$(CONFIG_SND_PLATFORM_SUNXI_PCM) += sunxi-pcm.o

# Common Platform Daudio
obj-$(CONFIG_SND_PLATFORM_SUNXI_DAUDIO) += sunxi-daudio.o

# Common Platform DMIC
obj-$(CONFIG_SND_PLATFORM_SUNXI_DMIC) += sunxi-dmic.o

# Common Platform SPDIF
obj-$(CONFIG_SND_PLATFORM_SUNXI_SPDIF) += sunxi-spdif.o

# Common Platform MAD
obj-$(CONFIG_SND_PLATFORM_SUNXI_MAD) += sunxi-mad.o
ifneq (, $(filter y m, $(CONFIG_ARCH_SUN20IW2)))
obj-$(CONFIG_SND_PLATFORM_SUNXI_MAD) += platforms/mad-sun20iw2.o
endif

# Sun8iw19p1 Daudio
obj-$(CONFIG_SND_PLATFORM_SUN8IW19_DAUDIO) += sun8iw19-daudio.o

