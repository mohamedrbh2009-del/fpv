LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := FallSoft
LOCAL_CPP_EXTENSION := .cpp .cc
LOCAL_SRC_FILES := main.cpp mod/logger.cpp
LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_CFLAGS    += -O2 -mfloat-abi=softfp -DNDEBUG
LOCAL_CPPFLAGS  += -std=c++17 -fexceptions -frtti
LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)
