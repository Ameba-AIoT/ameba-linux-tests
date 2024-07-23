LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := rtk_captouch_test
LOCAL_CFLAGS := #-Werror
LOCAL_SRC_FILES := rtk_captouch_test.c
LOCAL_C_INCLUDES := \
	prebuilts/kernel/include

include $(BUILD_EXECUTABLE)
