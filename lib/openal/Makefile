OPENAL_DIR = $(NXDK_DIR)/lib/openal
OPENAL_SOFT_DIR = $(OPENAL_DIR)/openal-soft

OPENAL_SOFT_COMMON_SRCS = \
    $(OPENAL_SOFT_DIR)/common/alcomplex.c \
    $(OPENAL_SOFT_DIR)/common/almalloc.c \
    $(OPENAL_SOFT_DIR)/common/atomic.c \
    $(OPENAL_SOFT_DIR)/common/rwlock.c \
    $(OPENAL_SOFT_DIR)/common/threads.c \
    $(OPENAL_SOFT_DIR)/common/uintmap.c

OPENAL_SOFT_OPENAL_SRCS = \
    $(OPENAL_SOFT_DIR)/OpenAL32/alAuxEffectSlot.c \
    $(OPENAL_SOFT_DIR)/OpenAL32/alBuffer.c \
    $(OPENAL_SOFT_DIR)/OpenAL32/alEffect.c \
    $(OPENAL_SOFT_DIR)/OpenAL32/alError.c \
    $(OPENAL_SOFT_DIR)/OpenAL32/alExtension.c \
    $(OPENAL_SOFT_DIR)/OpenAL32/alFilter.c \
    $(OPENAL_SOFT_DIR)/OpenAL32/alListener.c \
    $(OPENAL_SOFT_DIR)/OpenAL32/alSource.c \
    $(OPENAL_SOFT_DIR)/OpenAL32/alState.c \
    $(OPENAL_SOFT_DIR)/OpenAL32/event.c \
    $(OPENAL_SOFT_DIR)/OpenAL32/sample_cvt.c

OPENAL_SOFT_ALC_SRCS = \
    $(OPENAL_SOFT_DIR)/Alc/ALc.c \
    $(OPENAL_SOFT_DIR)/Alc/ALu.c \
    $(OPENAL_SOFT_DIR)/Alc/alconfig.c \
    $(OPENAL_SOFT_DIR)/Alc/bs2b.c \
    $(OPENAL_SOFT_DIR)/Alc/converter.c \
    $(OPENAL_SOFT_DIR)/Alc/mastering.c \
    $(OPENAL_SOFT_DIR)/Alc/ringbuffer.c \
    $(OPENAL_SOFT_DIR)/Alc/effects/chorus.c \
    $(OPENAL_SOFT_DIR)/Alc/effects/compressor.c \
    $(OPENAL_SOFT_DIR)/Alc/effects/dedicated.c \
    $(OPENAL_SOFT_DIR)/Alc/effects/distortion.c \
    $(OPENAL_SOFT_DIR)/Alc/effects/echo.c \
    $(OPENAL_SOFT_DIR)/Alc/effects/equalizer.c \
    $(OPENAL_SOFT_DIR)/Alc/effects/fshifter.c \
    $(OPENAL_SOFT_DIR)/Alc/effects/modulator.c \
    $(OPENAL_SOFT_DIR)/Alc/effects/null.c \
    $(OPENAL_SOFT_DIR)/Alc/effects/pshifter.c \
    $(OPENAL_SOFT_DIR)/Alc/effects/reverb.c \
    $(OPENAL_SOFT_DIR)/Alc/filters/filter.c \
    $(OPENAL_SOFT_DIR)/Alc/filters/nfc.c \
    $(OPENAL_SOFT_DIR)/Alc/filters/splitter.c \
    $(OPENAL_DIR)/helpers.c \
    $(OPENAL_SOFT_DIR)/Alc/hrtf.c \
    $(OPENAL_SOFT_DIR)/Alc/uhjfilter.c \
    $(OPENAL_SOFT_DIR)/Alc/ambdec.c \
    $(OPENAL_SOFT_DIR)/Alc/bformatdec.c \
    $(OPENAL_SOFT_DIR)/Alc/panning.c \
    $(OPENAL_SOFT_DIR)/Alc/mixvoice.c \
    $(OPENAL_SOFT_DIR)/Alc/mixer/mixer_c.c

OPENAL_SOFT_SRCS = \
    $(OPENAL_SOFT_COMMON_SRCS) \
    $(OPENAL_SOFT_OPENAL_SRCS) \
    $(OPENAL_SOFT_ALC_SRCS) \
    $(OPENAL_SOFT_DIR)/Alc/mixer/mixer_sse.c

OPENAL_SOFT_OBJS = $(addsuffix .obj, $(basename $(OPENAL_SOFT_SRCS)))

NXDK_CFLAGS := \
    -I $(OPENAL_DIR) \
    -I $(OPENAL_SOFT_DIR) \
    -iquote $(OPENAL_SOFT_DIR)/common \
    -I $(OPENAL_SOFT_DIR)/OpenAL32/Include \
    -I $(OPENAL_SOFT_DIR)/Alc \
    -I $(OPENAL_SOFT_DIR)/include \
    ${NXDK_CFLAGS} \
    -idirafter $(OPENAL_SOFT_DIR)/common

NXDK_CFLAGS += -DAL_LIBTYPE_STATIC
NXDK_CFLAGS += -DHAVE_STRUCT_TIMESPEC

NXDK_CFLAGS += -Dstrcasecmp=_stricmp
NXDK_CFLAGS += -Dstrncasecmp=_strnicmp

#FIXME: Move to winapi
NXDK_CFLAGS += -DDebugBreak\(\)=assert\(0\)
NXDK_CFLAGS += -DEXCEPTION_CONTINUE_EXECUTION=-1
NXDK_CFLAGS += -DHMODULE=void*
NXDK_CFLAGS += -DAPIENTRY=
NXDK_CFLAGS += -DWIN32_FIND_DATAW=WIN32_FIND_DATAA
NXDK_CFLAGS += -DHINSTANCE=void*
NXDK_CFLAGS += -DDLL_PROCESS_ATTACH=0
NXDK_CFLAGS += -DDLL_THREAD_ATTACH=1
NXDK_CFLAGS += -DDLL_THREAD_DETACH=2
NXDK_CFLAGS += -DDLL_PROCESS_DETACH=3

#NXDK_CXXFLAGS += -DCreateFileW\(...\)=INVALID_HANDLE_VALUE
#NXDK_CXXFLAGS += -DLoadLibraryW\(...\)=NULL
#NXDK_CXXFLAGS += -DGetProcAddress\(...\)=NULL
#NXDK_CXXFLAGS += -DFreeLibrary\(...\)=0
#NXDK_CXXFLAGS += -D_wgetenv\(...\)=NULL
#NXDK_CXXFLAGS += -DWideCharToMultiByte\(...\)=0
#NXDK_CXXFLAGS += -DMultiByteToWideChar\(...\)=0

$(NXDK_DIR)/lib/libopenal.lib: $(OPENAL_SOFT_OBJS)

main.exe: $(NXDK_DIR)/lib/libopenal.lib

CLEANRULES += clean-openal

.PHONY: clean-openal
clean-openal:
	$(VE)rm -f $(OPENAL_SOFT_OBJS) $(NXDK_DIR)/lib/libopenal.lib
