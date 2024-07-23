LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE:= tinymembench
LOCAL_CFLAGS := -Werror
LOCAL_SRC_FILES := \
    aarch64-asm.S \
    arm-neon.S \
    asm-opt.c \
    mips-32.S \
    x86-sse2.S \
    util.c \
    main.c

include $(BUILD_EXECUTABLE)