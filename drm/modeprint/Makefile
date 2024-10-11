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

exe-y = rtk_drm_mode_print

all: $(O)/$(exe-y)

$(O)/$(exe-y):
	$(CC) $(CFLAGS) $(LIBDRM_INC) $(LIBDRM_LIB) $(LDFLAGS) modeprint.c ../util/format.c ../util/kms.c ../util/pattern.c -o $@

clean:
	-rm -f $(O)/*