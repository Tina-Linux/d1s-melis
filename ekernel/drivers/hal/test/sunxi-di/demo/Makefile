#This confidential and proprietary software should be used
#under the licensing agreement from Allwinner Technology.

#Copyright (C) 2020-2030 Allwinner Technology Limited
#All rights reserved.

#Author: zhengwanyu <zhengwanyu@allwinnertech.com>

#The entire notice above must be reproduced on all authorised
#copies and copies may only be made to the extent permitted
#by a licensing agreement from Allwinner Technology Limited.

CROSS_COMPILE := /home/liborong/longan/out/gcc-linaro-5.3.1-2016.05-x86_64_arm-linux-gnueabi/bin/arm-linux-gnueabi-


LIBS_DIR := -L..
HEADERS_DIR :=

CC := $(CROSS_COMPILE)gcc
LIBS := $(LIBS_DIR)
INCLUDE := $(HEADERS_DIR)

LDFLAGS := -lDeinterlaceHal
DEFINES :=

CFLAGS := -g -Wall -O3 $(DEFINES) $(INCLUDE) $(CFLAGS)

SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))

di_demo:$(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

clean :
	rm -f $(OBJS)
	rm -f di_demo
