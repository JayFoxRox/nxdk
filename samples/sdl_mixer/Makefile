XBE_TITLE = sdl_mixer
GEN_XISO = $(XBE_TITLE).iso
SRCS += $(CURDIR)/main.c
NXDK_DIR = $(CURDIR)/../..
NXDK_SDL = y

all_local: cp_music all

include $(NXDK_DIR)/Makefile

cp_music:
	@mkdir -p $(OUTPUT_DIR)
	cp song.ogg $(OUTPUT_DIR)/
