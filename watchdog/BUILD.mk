LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := rtk_watchdog_test
LOCAL_CFLAGS := #-Werror
LOCAL_SRC_FILES := watchdog.c
LOCAL_C_INCLUDES := prebuilts/kernel/include

include $(BUILD_EXECUTABLE)
