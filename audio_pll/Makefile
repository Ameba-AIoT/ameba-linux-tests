#
# Realtek Semiconductor Corp.
#

CROSS_COMPILE ?= arm-linux-

CC := $(CROSS_COMPILE)gcc
CXX := $(CROSS_COMPILE)g++
LD := $(CROSS_COMPILE)gcc
AR := $(CROSS_COMPILE)ar
RANLIB := $(CROSS_COMPILE)ranlib
O ?= $(shell pwd)

CFLAGS += "-I./include"
CFLAGS += "-I../../kernel/linux-6.6/include/uapi"

exe-y = rtk_audio_pll_test

all: $(O)/$(exe-y)

$(O)/$(exe-y):
	$(CC) $(CFLAGS) $(LDFLAGS) audio_pll.c -o $@

clean:
	-rm -f $(O)/*
