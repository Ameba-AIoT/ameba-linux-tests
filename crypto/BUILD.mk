LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := kmod-crypto-test
LOCAL_MODULE_PATH := $(TARGET_ROOT_OUT)/lib/modules/5.4.63/
LOCAL_KERNAL_EXTMOD := $(LOCAL_PATH)
LOCAL_SRC_FILES := realtek-crypto-test.c

include $(BUILD_KERNEL_MODULE)