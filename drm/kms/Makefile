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

exe-y = rtk_drm_libkms_test

all: $(O)/$(exe-y)

$(O)/$(exe-y):
	$(CC) $(CFLAGS) $(LIBDRM_INC) $(LIBDRM_LIB) $(LDFLAGS) kms-universal-planes.c libkms-test-crtc.c libkms-test-device.c  libkms-test-framebuffer.c libkms-test-plane.c libkms-test-screen.c ../util/format.c ../util/kms.c ../util/pattern.c -o $@

clean:
	-rm -f $(O)/*