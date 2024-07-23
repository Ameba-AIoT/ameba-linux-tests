LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := rtk_hello_test
LOCAL_CFLAGS := #-Werror
LOCAL_SRC_FILES := hello.c

include $(BUILD_EXECUTABLE)
