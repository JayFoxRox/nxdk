NXDK_DIR = $(CURDIR)/../..
NXDK_RDT_DIR = $(NXDK_DIR)/../nxdk-rdt

XBE_TITLE = textures
GEN_XISO = $(XBE_TITLE).iso
SRCS = $(CURDIR)/main.c
SHADER_OBJS = ps.inl vs.inl




# The following is from nxdk-rdt, but adapted

NXDK_NET = y

GEN_XISO = $(XBE_TITLE).iso

SRCS += $(NXDK_RDT_DIR)/net.c
SRCS += $(NXDK_RDT_DIR)/dbgd.c
SRCS += $(NXDK_RDT_DIR)/lib/protobuf-c/protobuf-c.c
SRCS += $(NXDK_RDT_DIR)/dbg.pb-c.c # Might not exist when the wildcard above runs


CFLAGS += -I$(CURDIR)/
CFLAGS += -I$(NXDK_RDT_DIR)/
CFLAGS += -I$(NXDK_RDT_DIR)/lib

all_local: all

include $(NXDK_DIR)/Makefile

#FIXME: Needs to be in different dir
#.PHONY:clean_local
#clean_local:
#	rm -f *.inl* log.txt dump.cap main.lib *.pb-c.c *.pb-c.h *_pb2.py

#clean: clean_local

$(NXDK_RDT_DIR)/%.pb-c.c $(NXDK_RDT_DIR)/%.pb-c.h: $(NXDK_RDT_DIR)/%.proto
	protoc-c --c_out=$(NXDK_RDT_DIR)/ "$<" --proto_path $(NXDK_RDT_DIR)
