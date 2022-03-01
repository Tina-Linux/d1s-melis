obj-y += fat_cache.o
obj-y += fat_dir.o
obj-y += fat_entry.o
obj-y += fat_file.o
obj-y += fat_format.o
obj-y += fatfs.o
obj-y += fat_inode.o
obj-y += fat_misc.o
obj-y += fat_namei.o

CFLAGS_fat_dir.o := -fshort-wchar

ifeq ($(CONFIG_ARM), y)
EXTRA_LDFLAGS := --no-wchar-size-warning
endif
