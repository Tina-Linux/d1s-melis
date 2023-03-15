ccflags-y += 	-I$(obj)/ \
		-I$(srctree)/include/melis/kernel/drivers

obj-y += clk.o
obj-y += ccu.o
obj-y += ccu_mux.o
obj-y += ccu_nm.o
obj-y += ccu_common.o
obj-y += ccu_reset.o
obj-y += ccu_div.o
obj-y += ccu_frac.o
obj-y += ccu_gate.o
obj-y += ccu_mp.o
obj-y += ccu_mult.o
obj-y += ccu_nk.o
obj-y += ccu_nkm.o
obj-y += ccu_nkmp.o
obj-y += ccu_sdm.o
obj-y += clk-fixed-factor.o
obj-y += clk-fixed-rate.o
obj-y += clk-divider.o
obj-$(CONFIG_ARCH_SUN8IW20) += ccu-sun8iw20.o
obj-$(CONFIG_ARCH_SUN8IW20) += ccu-sun8iw20-r.o
obj-$(CONFIG_ARCH_SUN8IW20) += ccu-sun8iw20-rtc.o

obj-$(CONFIG_SOC_SUN20IW1) += ccu-sun8iw20.o
obj-$(CONFIG_SOC_SUN20IW1) += ccu-sun8iw20-r.o
obj-$(CONFIG_SOC_SUN20IW1) += ccu-sun8iw20-rtc.o

obj-$(CONFIG_ARCH_SUN20IW2) += ccu-sun20iw2.o
obj-$(CONFIG_ARCH_SUN20IW2) += ccu-sun20iw2-r.o
obj-$(CONFIG_ARCH_SUN20IW2) += ccu-sun20iw2-aon.o

obj-$(CONFIG_ARCH_SUN55IW3) += ccu-sun55iw3.o
obj-$(CONFIG_ARCH_SUN55IW3) += ccu-sun55iw3-dsp.o
obj-$(CONFIG_ARCH_SUN55IW3) += ccu-sun55iw3-r.o

ifeq ($(CONFIG_COMPONENTS_PM), y)
CFLAGS += -Icomponents/common/aw/pm/include/
endif
