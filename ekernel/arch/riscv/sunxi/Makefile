ccflags-y += -I$(srctree)/include/melis/kernel/drivers

obj-y += startup.o
obj-y += init.o
obj-y += va2pa.o
obj-y += blobdata.o

#force build blobdata.S .FORCE will always be create so blobdata would re-create
#every time.
$(obj)/blobdata.o: .FORCE

.FORCE:
