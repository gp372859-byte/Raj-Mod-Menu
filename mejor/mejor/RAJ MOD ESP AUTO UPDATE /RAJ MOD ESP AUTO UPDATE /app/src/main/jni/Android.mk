LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

# Output library name:
# libTeddyModder.so
LOCAL_MODULE := RajModder

# C/C++ compiler options
LOCAL_CFLAGS := \
    -w \
    -s \
    -Wno-error=format-security \
    -fvisibility=hidden \
    -fpermissive \
    -fexceptions

LOCAL_CPPFLAGS := \
    -w \
    -s \
    -Wno-error=format-security \
    -fvisibility=hidden \
    -std=c++17 \
    -Wno-error=c++11-narrowing \
    -fpermissive \
    -Wall \
    -fexceptions

# Linker options
LOCAL_LDFLAGS := -Wl,--gc-sections,--strip-all

# Android system libraries
LOCAL_LDLIBS := \
    -llog \
    -landroid \
    -lEGL \
    -lGLESv2

# ARM mode
LOCAL_ARM_MODE := arm

# Include current project directory
LOCAL_C_INCLUDES += $(LOCAL_PATH)

# Source files
LOCAL_SRC_FILES := \
    Main.cpp \
    Includes/MonoString.cpp \
    Substrate/hde64.c \
    Substrate/SubstrateDebug.cpp \
    Substrate/SubstrateHook.cpp \
    Substrate/SubstratePosixMemory.cpp \
    Substrate/SymbolFinder.cpp \
    KittyMemory/KittyMemory.cpp \
    KittyMemory/MemoryPatch.cpp \
    KittyMemory/MemoryBackup.cpp \
    KittyMemory/KittyUtils.cpp

include $(BUILD_SHARED_LIBRARY)
