XBE_TITLE=simple-theora-video-decoder
GEN_XISO = $(XBE_TITLE).iso
SRCS = $(wildcard $(CURDIR)/*.c)
NXDK_DIR = $(CURDIR)/../..
#CFLAGS=-I$(NXDK_DIR)/lib/libvpx -I$(NXDK_DIR)/lib/libvpx/libvpx
NXDK_SDL = y

all_local: cp_video all

include $(NXDK_DIR)/Makefile

cp_video:
	@mkdir -p $(OUTPUT_DIR)
	cp input.ogg $(OUTPUT_DIR)/
