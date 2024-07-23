LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := rtk_sdioh_test
LOCAL_CFLAGS := -Werror
LOCAL_SRC_FILES := sdioh.c

include $(BUILD_EXECUTABLE)
