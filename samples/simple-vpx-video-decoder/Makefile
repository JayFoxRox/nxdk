XBE_TITLE=simple-vpx-video-decoder
GEN_XISO = $(XBE_TITLE).iso
SRCS = \
  $(CURDIR)/main.cpp \
  $(CURDIR)/video_reader.c
NXDK_DIR = $(CURDIR)/../..
CXXFLAGS+=-I$(NXDK_DIR)/lib/libvpx -I$(NXDK_DIR)/lib/libvpx/libvpx
CXXFLAGS+=-I$(NXDK_DIR)/lib/libyuv/libyuv/include
NXDK_SDL = y

all_local: cp_video all

include $(NXDK_DIR)/Makefile

cp_video:
	@mkdir -p $(OUTPUT_DIR)
	cp input.ivf $(OUTPUT_DIR)/
