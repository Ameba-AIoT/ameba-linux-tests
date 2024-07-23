LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := rtk_ir_test
LOCAL_CFLAGS := #-Werror
LOCAL_SRC_FILES := ir.c

include $(BUILD_EXECUTABLE)
