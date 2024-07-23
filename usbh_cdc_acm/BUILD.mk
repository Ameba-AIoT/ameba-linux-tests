LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := rtk_usbh_cdc_acm_test
LOCAL_CFLAGS := #-Werror
LOCAL_SRC_FILES := usbh_cdc_acm.c
LOCAL_C_INCLUDES := \
	prebuilts/kernel/include

include $(BUILD_EXECUTABLE)
