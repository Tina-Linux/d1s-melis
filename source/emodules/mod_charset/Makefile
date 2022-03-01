subdir-ccflags-y +=		-I$(srctree)/include/melis/kernel/drivers  \
				-I$(srctree)/include/melis   \
				-I$(srctree)/include/melis/module   \
				-I$(srctree)/emodules/mod_charset    \
				-I$(srctree)/include/melis/kernel    \
				-DEPDK_DEBUG_LEVEL=EPDK_DEBUG_LEVEL_LOG_ALL -DUSED_BY_CHARSET 
			
usrlibs-y +=			--start-group \
				-lsyscall -lminic \
				--end-group 

MOD_NAME := charset
SUF_NAME := mod
PRJ_PATH := $(srctree)/projects/${TARGET_BOARD}/data/UDISK/mod/
TEMP_PATH := $(srctree)/emodules/bin

$(MOD_NAME)-objs += mod_charset.o
$(MOD_NAME)-objs += magic.o
$(MOD_NAME)-objs += built-in.o
obj-y := apitable/api_table_i.o
obj-$(CONFIG_CHARSET_UTF8_EN) += utf8/utf8.o
obj-$(CONFIG_CHARSET_UTF16BE_EN) += utf16be/utf16be.o
obj-$(CONFIG_CHARSET_UTF16LE_EN) += utf16le/utf16le.o
obj-$(CONFIG_CHARSET_BIG5_EN) += big5/big5.o
obj-$(CONFIG_CHARSET_GBK_EN) += gbk/gbk.o
obj-$(CONFIG_CHARSET_SJIS_EN) += sjis/sjis.o
obj-$(CONFIG_CHARSET_EUC_KR_EN) += euc_kr/euc_kr.o
obj-$(CONFIG_CHARSET_KIO8_R_EN) += kio8_r/kio8_r.o
obj-$(CONFIG_CHARSET_ISO_8859_1_EN) += iso8859_1/iso8859_1.o
obj-$(CONFIG_CHARSET_ISO_8859_2_EN) += iso8859_2/iso8859_2.o
obj-$(CONFIG_CHARSET_ISO_8859_3_EN) += iso8859_3/iso8859_3.o
obj-$(CONFIG_CHARSET_ISO_8859_4_EN) += iso8859_4/iso8859_4.o
obj-$(CONFIG_CHARSET_ISO_8859_5_EN) += iso8859_5/iso8859_5.o
obj-$(CONFIG_CHARSET_ISO_8859_6_EN) += iso8859_6/iso8859_6.o
obj-$(CONFIG_CHARSET_ISO_8859_7_EN) += iso8859_7/iso8859_7.o
obj-$(CONFIG_CHARSET_ISO_8859_8_EN) += iso8859_8/iso8859_8.o
obj-$(CONFIG_CHARSET_ISO_8859_9_EN) += iso8859_9/iso8859_9.o
obj-$(CONFIG_CHARSET_ISO_8859_10_EN) += iso8859_10/iso8859_10.o
obj-$(CONFIG_CHARSET_ISO_8859_11_EN) += iso8859_11/iso8859_11.o
obj-$(CONFIG_CHARSET_ISO_8859_13_EN) += iso8859_13/iso8859_13.o
obj-$(CONFIG_CHARSET_ISO_8859_14_EN) += iso8859_14/iso8859_14.o
obj-$(CONFIG_CHARSET_ISO_8859_15_EN) += iso8859_15/iso8859_15.o
obj-$(CONFIG_CHARSET_ISO_8859_16_EN) += iso8859_16/iso8859_16.o
obj-$(CONFIG_CHARSET_CP874_EN) += cp874/cp874.o
obj-$(CONFIG_CHARSET_CP1250_EN) += cp1250/cp1250.o
obj-$(CONFIG_CHARSET_CP1251_EN) += cp1251/cp1251.o
obj-$(CONFIG_CHARSET_CP1252_EN) += cp1252/cp1252.o
obj-$(CONFIG_CHARSET_CP1253_EN) += cp1253/cp1253.o
obj-$(CONFIG_CHARSET_CP1254_EN) += cp1254/cp1254.o
obj-$(CONFIG_CHARSET_CP1255_EN) += cp1255/cp1255.o
obj-$(CONFIG_CHARSET_CP1256_EN) += cp1256/cp1256.o
obj-$(CONFIG_CHARSET_CP1257_EN) += cp1257/cp1257.o

include $(MELIS_BASE)/scripts/Makefile.mods
