LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := rtk_adc_test
LOCAL_CFLAGS := -Werror
LOCAL_SRC_FILES := rtk_adc_test.c

include $(BUILD_EXECUTABLE)
