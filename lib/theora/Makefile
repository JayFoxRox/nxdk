THEORA_DIR := $(NXDK_DIR)/lib/theora/theora

CFLAGS += -isystem $(THEORA_DIR)/include

THEORADEC_SRCS := \
	$(THEORA_DIR)/lib/apiwrapper.c \
	$(THEORA_DIR)/lib/bitpack.c \
	$(THEORA_DIR)/lib/decapiwrapper.c \
	$(THEORA_DIR)/lib/decinfo.c \
	$(THEORA_DIR)/lib/decode.c \
	$(THEORA_DIR)/lib/dequant.c \
	$(THEORA_DIR)/lib/fragment.c \
	$(THEORA_DIR)/lib/huffdec.c \
	$(THEORA_DIR)/lib/idct.c \
	$(THEORA_DIR)/lib/info.c \
	$(THEORA_DIR)/lib/internal.c \
	$(THEORA_DIR)/lib/quant.c \
	$(THEORA_DIR)/lib/state.c \
	$(THEORA_DIR)/lib/x86/x86cpu.c \
	$(THEORA_DIR)/lib/x86/mmxidct.c \
	$(THEORA_DIR)/lib/x86/mmxfrag.c \
	$(THEORA_DIR)/lib/x86/mmxstate.c \
	$(THEORA_DIR)/lib/x86/sse2idct.c \
	$(THEORA_DIR)/lib/x86/x86state.c

THEORAENC_SRCS := \
	$(THEORA_DIR)/src/framing.c \
	$(THEORA_DIR)/src/bitwise.c

THEORADEC_OBJS = $(addsuffix .obj, $(basename $(THEORADEC_SRCS)))
THEORAENC_OBJS = $(addsuffix .obj, $(basename $(THEORAENC_SRCS)))

$(NXDK_DIR)/lib/libtheoradec.lib: $(THEORADEC_OBJS)
$(NXDK_DIR)/lib/libtheoraenc.lib: $(THEORAENC_OBJS)

main.exe: $(NXDK_DIR)/lib/libtheoradec.lib
#main.exe: $(NXDK_DIR)/lib/libtheoraenc.lib

CLEANRULES += clean-theora

.PHONY: clean-theora
clean-theora:
	$(VE)rm -f $(THEORADEC_OBJS) $(NXDK_DIR)/lib/libtheoradec.lib
	$(VE)rm -f $(THEORADEC_OBJS) $(NXDK_DIR)/lib/libtheoraenc.lib
