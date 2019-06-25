NXDK_DIR = $(CURDIR)/../..

XBE_TITLE = textures
GEN_XISO = $(XBE_TITLE).iso
SRCS = $(CURDIR)/main.c
SHADER_OBJS = ps.inl vs.inl




# The following is from nxdk-rdt, but adapted

NXDK_NET = y

GEN_XISO = $(XBE_TITLE).iso

SRCS += $(CURDIR)/nxdk-rdt/net.c
SRCS += $(CURDIR)/nxdk-rdt/dbgd.c

CFLAGS += -I$(CURDIR)/
CFLAGS += -I$(CURDIR)/nxdk-rdt/lib

all_local: all

include $(NXDK_DIR)/Makefile

#FIXME: Needs to be in different dir
#.PHONY:clean_local
#clean_local:
#	rm -f *.inl* log.txt dump.cap main.lib *.pb-c.c *.pb-c.h *_pb2.py

#clean: clean_local

$(CURDIR)/nxdk-rdt/%.pb-c.c $(CURDIR)/nxdk-rdt/%.pb-c.h: nxdk-rdt/%.proto
	protoc-c --c_out=$(CURDIR)/ "$<"
