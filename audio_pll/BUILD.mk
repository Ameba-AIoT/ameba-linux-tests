LOCAL_PATH:= $(call my-dir)

#######################################
# rtk_audio_pll_test
#######################################
include $(CLEAR_VARS)
LOCAL_MODULE := rtk_audio_pll_test
LOCAL_CFLAGS := -Werror

LOCAL_INCLUDES := \
                tests/audio_pll/include

LOCAL_SOURCES := \
                audio_pll.c

include $(BUILD_EXECUTABLE)

