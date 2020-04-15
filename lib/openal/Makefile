OPENAL_DIR = $(NXDK_DIR)/lib/openal
OPENAL_SOFT_DIR = $(OPENAL_DIR)/openal-soft

OPENAL_SOFT_COMMON_SRCS = \
    $(OPENAL_SOFT_DIR)/common/alcomplex.cpp \
    $(OPENAL_SOFT_DIR)/common/alexcpt.cpp  \
    $(OPENAL_SOFT_DIR)/common/alfstream.cpp \
    $(OPENAL_SOFT_DIR)/common/almalloc.cpp \
    $(OPENAL_SOFT_DIR)/common/alstring.cpp \
    $(OPENAL_SOFT_DIR)/common/dynload.cpp \
    $(OPENAL_SOFT_DIR)/common/polyphase_resampler.cpp \
    $(OPENAL_DIR)/strutils.cpp
    #$(OPENAL_SOFT_DIR)/common/threads.cpp

OPENAL_SOFT_OPENAL_SRCS = \
    $(OPENAL_SOFT_DIR)/al/auxeffectslot.cpp \
    $(OPENAL_SOFT_DIR)/al/buffer.cpp \
    $(OPENAL_SOFT_DIR)/al/effect.cpp \
    $(OPENAL_SOFT_DIR)/al/error.cpp \
    $(OPENAL_SOFT_DIR)/al/event.cpp \
    $(OPENAL_SOFT_DIR)/al/extension.cpp \
    $(OPENAL_SOFT_DIR)/al/filter.cpp \
    $(OPENAL_SOFT_DIR)/al/listener.cpp \
    $(OPENAL_SOFT_DIR)/al/source.cpp \
    $(OPENAL_SOFT_DIR)/al/state.cpp

OPENAL_SOFT_ALC_SRCS = \
    $(OPENAL_SOFT_DIR)/alc/alc.cpp \
    $(OPENAL_SOFT_DIR)/alc/alu.cpp \
    $(OPENAL_SOFT_DIR)/alc/alconfig.cpp \
    $(OPENAL_SOFT_DIR)/alc/ambdec.cpp \
    $(OPENAL_SOFT_DIR)/alc/bformatdec.cpp \
    $(OPENAL_SOFT_DIR)/alc/bs2b.cpp \
    $(OPENAL_SOFT_DIR)/alc/converter.cpp \
    $(OPENAL_SOFT_DIR)/alc/effects/autowah.cpp \
    $(OPENAL_SOFT_DIR)/alc/effects/chorus.cpp \
    $(OPENAL_SOFT_DIR)/alc/effects/compressor.cpp \
    $(OPENAL_SOFT_DIR)/alc/effects/dedicated.cpp \
    $(OPENAL_SOFT_DIR)/alc/effects/distortion.cpp \
    $(OPENAL_SOFT_DIR)/alc/effects/echo.cpp \
    $(OPENAL_SOFT_DIR)/alc/effects/equalizer.cpp \
    $(OPENAL_SOFT_DIR)/alc/effects/fshifter.cpp \
    $(OPENAL_SOFT_DIR)/alc/effects/modulator.cpp \
    $(OPENAL_SOFT_DIR)/alc/effects/null.cpp \
    $(OPENAL_SOFT_DIR)/alc/effects/pshifter.cpp \
    $(OPENAL_SOFT_DIR)/alc/effects/reverb.cpp \
    $(OPENAL_SOFT_DIR)/alc/effects/vmorpher.cpp \
    $(OPENAL_SOFT_DIR)/alc/filters/biquad.cpp \
    $(OPENAL_SOFT_DIR)/alc/filters/nfc.cpp \
    $(OPENAL_SOFT_DIR)/alc/filters/splitter.cpp \
    $(OPENAL_SOFT_DIR)/alc/helpers.cpp \
    $(OPENAL_SOFT_DIR)/alc/hrtf.cpp \
    $(OPENAL_SOFT_DIR)/alc/mastering.cpp \
    $(OPENAL_SOFT_DIR)/alc/panning.cpp \
    $(OPENAL_SOFT_DIR)/alc/ringbuffer.cpp \
    $(OPENAL_SOFT_DIR)/alc/uhjfilter.cpp \
    $(OPENAL_SOFT_DIR)/alc/uiddefs.cpp \
    $(OPENAL_SOFT_DIR)/alc/voice.cpp \
    $(OPENAL_SOFT_DIR)/alc/mixer/mixer_c.cpp

OPENAL_SOFT_SRCS = \
    $(OPENAL_SOFT_COMMON_SRCS) \
    $(OPENAL_SOFT_OPENAL_SRCS) \
    $(OPENAL_SOFT_ALC_SRCS)

OPENAL_SOFT_OBJS = $(addsuffix .obj, $(basename $(OPENAL_SOFT_SRCS)))

NXDK_CXXFLAGS += \
    -I $(OPENAL_DIR) \
    -I $(OPENAL_SOFT_DIR) \
    -I $(OPENAL_SOFT_DIR)/common \
    -I $(OPENAL_SOFT_DIR)/alc \
    -I $(OPENAL_SOFT_DIR)/include \

NXDK_CXXFLAGS += -DCreateFileW\(...\)=INVALID_HANDLE_VALUE
NXDK_CXXFLAGS += -DLoadLibraryW\(...\)=NULL
NXDK_CXXFLAGS += -DHMODULE=void*
NXDK_CXXFLAGS += -DGetProcAddress\(...\)=NULL
NXDK_CXXFLAGS += -DFreeLibrary\(...\)=0
NXDK_CXXFLAGS += -D_wgetenv\(...\)=NULL
NXDK_CXXFLAGS += -DWideCharToMultiByte\(...\)=0
NXDK_CXXFLAGS += -DMultiByteToWideChar\(...\)=0

NXDK_CXX = y

$(NXDK_DIR)/lib/libopenal.lib: $(OPENAL_SOFT_OBJS)

main.exe: $(NXDK_DIR)/lib/libopenal.lib

CLEANRULES += clean-openal

.PHONY: clean-openal
clean-openal:
	$(VE)rm -f $(OPENAL_SOFT_OBJS) $(NXDK_DIR)/lib/libopenal.lib
