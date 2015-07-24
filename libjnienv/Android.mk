LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := libjnienv
LOCAL_SRC_FILES := libjnienv.cpp
# LOCAL_CFLAGS := -I$(LOCAL_PATH)/../libc/
# LOCAL_STATIC_LIBRARIES := libc_nomalloc

include $(BUILD_SHARED_LIBRARY)
