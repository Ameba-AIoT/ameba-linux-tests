LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := rtk_uart_test
LOCAL_CFLAGS := #-Werror
LOCAL_SRC_FILES := uart.c
LOCAL_C_INCLUDES := \
	prebuilts/kernel/include

include $(BUILD_EXECUTABLE)
