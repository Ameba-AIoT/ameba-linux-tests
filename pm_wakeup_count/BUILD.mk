LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := rtk_pm_wakeup_count_test
LOCAL_CFLAGS := #-Werror
LOCAL_SRC_FILES := pm_wakeup_count.c
LOCAL_C_INCLUDES := \
	prebuilts/kernel/include

include $(BUILD_EXECUTABLE)
