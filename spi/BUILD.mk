LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := rtk_spi_test
LOCAL_CFLAGS := #-Werror
LOCAL_SRC_FILES := spi.c

include $(BUILD_EXECUTABLE)
