LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := libjnienv
LOCAL_SRC_FILES := libjnienv.cpp
# LOCAL_CFLAGS := -I$(LOCAL_PATH)/../libc/
LOCAL_STATIC_LIBRARIES := liblog
LOCAL_C_INCLUDES += external/stlport/stlport bionic/ bionic/libstdc++/include
LOCAL_SHARED_LIBRARIES += libstlport

include $(BUILD_SHARED_LIBRARY)
