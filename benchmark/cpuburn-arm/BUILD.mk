LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := cpuburn
LOCAL_CFLAGS := #-Werror
LOCAL_SRC_FILES := cpuburn-a7.S

include $(BUILD_EXECUTABLE)
