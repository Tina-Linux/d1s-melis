subdir-ccflags-y +=     -I$(srctree)/ekernel/filesystem      \
            			-I$(srctree)/ekernel/filesystem/dbg  \
            			-I$(srctree)/ekernel/filesystem/fs   \
            			-I$(srctree)/ekernel/filesystem/part \
            			-I$(srctree)/ekernel/filesystem/dbg  \
            			-I$(srctree)/ekernel/filesystem/include      \
            			-I$(srctree)/ekernel/filesystem/fs/cdfs    \
            			-I$(srctree)/ekernel/filesystem/fs/devfs   \
            			-I$(srctree)/ekernel/filesystem/fs/exfatfs \
            			-I$(srctree)/ekernel/filesystem/fs/fatfs   \
            			-I$(srctree)/ekernel/filesystem/fs/minfs   \
            			-I$(srctree)/ekernel/filesystem/fs/minfs/lzma \
            			-I$(srctree)/ekernel/filesystem/fs/udffs \
            			-Wno-address-of-packed-member

obj-y += sys_fsys.o
obj-y += fsys_libs.o
obj-y += dbg/
obj-y += fs/
obj-y += part/
obj-y += devman/
