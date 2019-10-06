# Compile this like this:
# ../../tools/fp20compiler/fp20compiler 3-ps.inl.aaaa_rgba > ps.inl && make; rm -f ps.inl


XBE_TITLE = triangle
GEN_XISO = $(XBE_TITLE).iso
SRCS = $(wildcard $(CURDIR)/*.c)
SHADER_OBJS = vs.inl
NXDK_DIR = $(CURDIR)/../..
include $(NXDK_DIR)/Makefile
