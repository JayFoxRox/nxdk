XBE_TITLE = color-lut
GEN_XISO = $(XBE_TITLE).iso
SRCS = $(CURDIR)/main.c
NXDK_DIR = $(CURDIR)/../..

include $(NXDK_DIR)/Makefile

$(CURDIR)/main.c: lut_r_gb_a.fp.inl lut_ar_gb.fp.inl pass_through.fp.inl

%.fp.inl: %.fp
	$(NXDK_DIR)/tools/fp20compiler/fp20compiler '$<' > '$@'
