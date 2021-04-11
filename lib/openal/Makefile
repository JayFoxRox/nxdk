OPENAL_DIR = $(NXDK_DIR)/lib/openal
OPENAL_SOFT_DIR = $(OPENAL_DIR)/openal-soft

OPENAL_SOFT_OPENAL_SRCS = \
    $(OPENAL_SOFT_DIR)/OpenAL32/alBuffer.c \
    $(OPENAL_SOFT_DIR)/OpenAL32/alError.c \
    $(OPENAL_SOFT_DIR)/OpenAL32/alExtension.c \
    $(OPENAL_SOFT_DIR)/OpenAL32/alListener.c \
    $(OPENAL_SOFT_DIR)/OpenAL32/alSource.c \
    $(OPENAL_SOFT_DIR)/OpenAL32/alState.c \
    $(OPENAL_SOFT_DIR)/OpenAL32/alThunk.c \
    $(OPENAL_SOFT_DIR)/OpenAL32/OpenAL32.c

OPENAL_SOFT_ALC_SRCS = \
    $(OPENAL_SOFT_DIR)/Alc/ALc.c \
    $(OPENAL_SOFT_DIR)/Alc/ALu.c \
    $(OPENAL_SOFT_DIR)/Alc/alcConfig.c \
    $(OPENAL_SOFT_DIR)/Alc/alcThread.c

OPENAL_SOFT_SRCS = \
    $(OPENAL_SOFT_OPENAL_SRCS) \
    $(OPENAL_SOFT_ALC_SRCS) \
    $(OPENAL_DIR)/xaudio.c

OPENAL_SOFT_OBJS = $(addsuffix .obj, $(basename $(OPENAL_SOFT_SRCS)))

NXDK_CFLAGS := \
    -I $(OPENAL_DIR) \
    -I $(OPENAL_SOFT_DIR) \
    -I $(OPENAL_SOFT_DIR)/OpenAL32/Include \
    -I $(OPENAL_SOFT_DIR)/Alc \
    -I $(OPENAL_SOFT_DIR)/include \
    ${NXDK_CFLAGS} \

#NXDK_CFLAGS += -D_XBOX
NXDK_CFLAGS += -DAL_BUILD_LIBRARY
#NXDK_CFLAGS += -DHAVE_STRUCT_TIMESPEC

NXDK_CFLAGS += -Dstrcasecmp=_stricmp
#NXDK_CFLAGS += -Dstrncasecmp=_strnicmp

#FIXME: Move to winapi

#NXDK_CFLAGS += -DtimeGetTime=GetTickCount
#NXDK_CFLAGS += -DWaitForSingleObject\(a,\ b\)=WaitForSingleObjectEx\(a,\ b,\ FALSE\)
NXDK_CFLAGS += -DDisableThreadLibraryCalls\(...\)=1
#NXDK_CFLAGS += -DGetExitCodeThread\(...\)=0
NXDK_CFLAGS += -DCALLBACK=__stdcall
#NXDK_CFLAGS += -DDebugBreak\(\)=assert\(0\)
#NXDK_CFLAGS += -DEXCEPTION_CONTINUE_EXECUTION=-1
#NXDK_CFLAGS += -DHMODULE=void*
NXDK_CFLAGS += -DAPIENTRY=
#NXDK_CFLAGS += -DWIN32_FIND_DATAW=WIN32_FIND_DATAA
#NXDK_CFLAGS += -DHINSTANCE=void*
NXDK_CFLAGS += -DDLL_PROCESS_ATTACH=0
#NXDK_CFLAGS += -DDLL_THREAD_ATTACH=1
#NXDK_CFLAGS += -DDLL_THREAD_DETACH=2
NXDK_CFLAGS += -DDLL_PROCESS_DETACH=3
#NXDK_CFLAGS += -DSHGetSpecialFolderPathA\(...\)=FALSE
NXDK_CFLAGS += -DIsBadWritePtr\(...\)=0

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
